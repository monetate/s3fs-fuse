######################################################################
# s3fs - FUSE-based file system backed by Amazon S3
#
# Copyright 2007-2008 Randy Rizun <rrizun@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
######################################################################
bin_PROGRAMS=s3fs

AM_CPPFLAGS = $(DEPS_CFLAGS)
if USE_GNUTLS_NETTLE
    AM_CPPFLAGS += -DUSE_GNUTLS_NETTLE
endif
if USE_SSL_OPENSSL_30
    AM_CPPFLAGS += -DUSE_OPENSSL_30
endif

s3fs_SOURCES = \
    s3fs.cpp \
    s3fs_global.cpp \
    s3fs_help.cpp \
    s3fs_logger.cpp \
    s3fs_xml.cpp \
    metaheader.cpp \
    mpu_util.cpp \
    curl.cpp \
    curl_share.cpp \
    curl_util.cpp \
    s3objlist.cpp \
    cache.cpp \
    cache_node.cpp \
    string_util.cpp \
    s3fs_cred.cpp \
    s3fs_util.cpp \
    s3fs_threadreqs.cpp \
    fdcache.cpp \
    fdcache_entity.cpp \
    fdcache_page.cpp \
    fdcache_stat.cpp \
    fdcache_auto.cpp \
    fdcache_fdinfo.cpp \
    fdcache_pseudofd.cpp \
    fdcache_untreated.cpp \
    addhead.cpp \
    sighandlers.cpp \
    threadpoolman.cpp \
    syncfiller.cpp \
    common_auth.cpp
if USE_SSL_OPENSSL
    s3fs_SOURCES += openssl_auth.cpp
endif
if USE_SSL_GNUTLS
    s3fs_SOURCES += gnutls_auth.cpp
endif
if USE_SSL_NSS
    s3fs_SOURCES += nss_auth.cpp
endif

s3fs_LDADD = $(DEPS_LIBS)

noinst_PROGRAMS = \
    test_curl_util \
    test_page_list \
    test_string_util

test_curl_util_SOURCES = common_auth.cpp curl_util.cpp string_util.cpp test_curl_util.cpp s3fs_global.cpp s3fs_logger.cpp
if USE_SSL_OPENSSL
    test_curl_util_SOURCES += openssl_auth.cpp
endif
if USE_SSL_GNUTLS
    test_curl_util_SOURCES += gnutls_auth.cpp
endif
if USE_SSL_NSS
    test_curl_util_SOURCES += nss_auth.cpp
endif

test_curl_util_LDADD = $(DEPS_LIBS)

test_page_list_SOURCES = \
    fdcache_page.cpp \
    s3fs_global.cpp \
    s3fs_logger.cpp \
    string_util.cpp \
    test_page_list.cpp

test_string_util_SOURCES = string_util.cpp test_string_util.cpp s3fs_logger.cpp

TESTS = \
    test_curl_util \
    test_page_list \
    test_string_util

clang-tidy:
	clang-tidy -extra-arg-before=-xc++ -extra-arg=-std=c++14 \
		*.h $(s3fs_SOURCES) test_curl_util.cpp test_page_list.cpp test_string_util.cpp \
		-- $(DEPS_CFLAGS) $(CPPFLAGS)

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim600: noexpandtab sw=4 ts=4 fdm=marker
# vim<600: noexpandtab sw=4 ts=4
#
