#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#include <hash.h>
#include <log.h>
#include <cache.h>
#include <disk_info.h>

static int hdc_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char* new_path = get_hd_path(path);
	res = lstat(new_path, stbuf);
	free(new_path);
	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_access(const char *path, int mask)
{
	int res;
	char* new_path = get_hd_path(path);
	res = access(new_path, mask);
	free(new_path);
	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char* new_path = get_hd_path(path);
	res = readlink(new_path, buf, size - 1);
	free(new_path);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int hdc_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;
	
	char* new_path = get_hd_path(path);
	dp = opendir(new_path);
	free(new_path);

	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int hdc_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	char* new_path = get_hd_path(path);
			
	if (S_ISREG(mode)) {
		res = open(new_path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(new_path, mode);
	else
		res = mknod(new_path, mode, rdev);
	free(new_path);
	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_mkdir(const char *path, mode_t mode)
{
	int res;
	char* new_path = get_hd_path(path);
	res = mkdir(new_path, mode);
	free(new_path);

	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_unlink(const char *path)
{
	int res;
	char* new_path = get_hd_path(path);
	res = unlink(new_path);
	free(new_path);

	if(cache_exists(path)) cache_remove(path);

	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_rmdir(const char *path)
{
	int res;
	char* new_path = get_hd_path(path);
	res = rmdir(new_path);
	free(new_path);

	if(cache_exists(path)){
		char* ssd_path = get_ssd_path(path);
		rmdir(ssd_path);
		free(ssd_path);
	}		
	if (res == -1)
		return -errno;

	return 0;
}

//TODO: fix for use with HD PATH
static int hdc_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}
//TODO: fix for use with HD PATH
static int hdc_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

//TODO: fix for use with HD PATH
static int hdc_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_chmod(const char *path, mode_t mode)
{
	int res;
	char* new_path = get_hd_path(path);
	res = chmod(new_path, mode);
	free(new_path);
	
	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char* new_path = get_hd_path(path);
	res = lchown(new_path, uid, gid);
	free(new_path);
	
	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_truncate(const char *path, off_t size)
{
	int res;
	char* new_path = get_hd_path(path);
	res = truncate(new_path, size);
	free(new_path);

	if (res == -1)
		return -errno;

	return 0;
}

#ifdef HAVE_UTIMENSAT
static int hdc_utimens(const char *path, const struct timespec ts[2])
{
	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}
#endif

static int hdc_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char* new_path = get_hd_path(path);
	res = open(new_path, fi->flags);
	free(new_path);

	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int hdc_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;
	
	char* new_path;
	log_msg("read");
	if(cache_exists(path)){
		new_path = get_ssd_path(path);
		log_msg(new_path);
	}
	else{
		new_path = get_hd_path(path);
		log_msg(new_path);
		cache_add(path);
	}

	fd = open(new_path, O_RDONLY);

	(void) fi;
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	free(new_path);
	return res;
}

static int hdc_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	
	char* new_path = get_hd_path(path);
	fd = open(new_path, O_WRONLY);

	(void) fi;
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	log_msg("write");
	log_msg(new_path);
	free(new_path);
	cache_add(path);
	return res;
}

static int hdc_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char* new_path = get_hd_path(path);
	res = statvfs(new_path, stbuf);
	free(new_path);

	if (res == -1)
		return -errno;

	return 0;
}

static int hdc_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) fi;
	return 0;
}

static int hdc_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int hdc_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;

	if (mode)
		return -EOPNOTSUPP;

	fd = open(path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = -posix_fallocate(fd, offset, length);

	close(fd);
	return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int hdc_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int hdc_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int hdc_listxattr(const char *path, char *list, size_t size)
{
	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int hdc_removexattr(const char *path, const char *name)
{
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations hdc_oper = {
	.getattr	= hdc_getattr,
	.access		= hdc_access,
	.readlink	= hdc_readlink,
	.readdir	= hdc_readdir,
	.mknod		= hdc_mknod,
	.mkdir		= hdc_mkdir,
	.symlink	= hdc_symlink,
	.unlink		= hdc_unlink,
	.rmdir		= hdc_rmdir,
	.rename		= hdc_rename,
	.link		= hdc_link,
	.chmod		= hdc_chmod,
	.chown		= hdc_chown,
	.truncate	= hdc_truncate,
#ifdef HAVE_UTIMENSAT
	.utimens	= hdc_utimens,
#endif
	.open		= hdc_open,
	.read		= hdc_read,
	.write		= hdc_write,
	.statfs		= hdc_statfs,
	.release	= hdc_release,
	.fsync		= hdc_fsync,
#ifdef HAVE_POSIX_FALLOCATE
	.fallocate	= hdc_fallocate,
#endif
#ifdef HAVE_SETXATTR
	.setxattr	= hdc_setxattr,
	.getxattr	= hdc_getxattr,
	.listxattr	= hdc_listxattr,
	.removexattr	= hdc_removexattr,
#endif
};

int main()//int argc, char *argv[])
{
	umask(0);
	char* argv[2] = { "./fusehdc", "../fs" };
	return fuse_main(2, argv, &hdc_oper, NULL);
}
