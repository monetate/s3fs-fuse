/*
 * s3fs - FUSE-based file system backed by Amazon S3
 *
 * Copyright(C) 2007 Randy Rizun <rrizun@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef S3FS_THREADREQS_H_
#define S3FS_THREADREQS_H_

#include <string>

#include "common.h"
#include "metaheader.h"
#include "curl.h"
#include "s3objlist.h"
#include "syncfiller.h"
#include "psemaphore.h"

//-------------------------------------------------------------------
// Structures for MultiThread Request
//-------------------------------------------------------------------
typedef std::map<std::string, int> retrycnt_t;

//
// Head Request parameter structure for Thread Pool.
//
struct head_req_thparam
{
    std::string path;
    headers_t*  pmeta  = nullptr;
    int         result = 0;
};

//
// Multi Head Request parameter structure for Thread Pool.
//
struct multi_head_req_thparam
{
    std::string   path;
    SyncFiller*   psyncfiller    = nullptr;
    std::mutex*   pthparam_lock  = nullptr;
    int*          pretrycount    = nullptr;
    s3obj_list_t* pnotfound_list = nullptr;
    bool          use_wtf8       = false;
    objtype_t     objtype        = objtype_t::UNKNOWN;
    int*          presult        = nullptr;
};

//
// Delete Request parameter structure for Thread Pool.
//
struct delete_req_thparam
{
    std::string path;
    int         result = 0;
};

//
// Put Head Request parameter structure for Thread Pool.
//
struct put_head_req_thparam
{
    std::string path;
    headers_t   meta;
    bool        isCopy = false;
    int         result = 0;
};

//
// Put Request parameter structure for Thread Pool.
//
struct put_req_thparam
{
    std::string path;
    headers_t   meta;
    int         fd     = -1;
    bool        ahbe   = false;
    int         result = 0;
};

//
// List Bucket Request parameter structure for Thread Pool.
//
struct list_bucket_req_thparam
{
    std::string  path;
    std::string  query;
    std::string* presponseBody = nullptr;
    int          result        = 0;
};

//
// Check Service Request parameter structure for Thread Pool.
//
struct check_service_req_thparam
{
    std::string  path;
    bool         forceNoSSE         = false;
    bool         support_compat_dir = false;
    long*        presponseCode      = nullptr;
    std::string* presponseBody      = nullptr;
    int          result             = 0;
};

//
// Pre Multipart Upload Request parameter structure for Thread Pool.
//
struct pre_multipart_upload_req_thparam
{
    std::string path;
    headers_t   meta;
    std::string upload_id;
    int         result = 0;
};

//
// Multipart Upload Part Request parameter structure for Thread Pool.
//
struct multipart_upload_part_req_thparam
{
    std::string   path;
    std::string   upload_id;
    int           upload_fd      = -1;
    off_t         start          = 0;
    off_t         size           = 0;
    bool          is_copy        = false;
    int           part_num       = -1;
    std::mutex*   pthparam_lock  = nullptr;
    etagpair*     petag          = nullptr;
    int*          presult        = nullptr;
};

//
// Complete Multipart Upload Request parameter structure for Thread Pool.
//
struct complete_multipart_upload_req_thparam
{
    std::string path;
    std::string upload_id;
    etaglist_t  etaglist;
    int         result = 0;
};

//
// Abort Multipart Upload Request parameter structure for Thread Pool.
//
struct abort_multipart_upload_req_thparam
{
    std::string path;
    std::string upload_id;
    int         result = 0;
};

//
// Multipart Put Head Request parameter structure for Thread Pool.
//
struct multipart_put_head_req_thparam
{
    std::string from;
    std::string to;
    std::string upload_id;
    int         part_number   = 0;
    headers_t   meta;
    std::mutex* pthparam_lock = nullptr;
    filepart*   ppartdata     = nullptr;
    int*        pretrycount   = nullptr;
    int*        presult       = nullptr;
};

//
// Parallel Get Object Request parameter structure for Thread Pool.
//
struct parallel_get_object_req_thparam
{
    std::string path;
    int         fd            = -1;
    off_t       start         = 0;
    off_t       size          = 0;
    sse_type_t  ssetype       = sse_type_t::SSE_DISABLE;
    std::string ssevalue;
    std::mutex* pthparam_lock = nullptr;
    int*        pretrycount   = nullptr;
    int*        presult       = nullptr;
};

//
// Get Object Request parameter structure for Thread Pool.
//
struct get_object_req_thparam
{
    std::string path;
    int         fd     = -1;
    off_t       start  = 0;
    off_t       size   = 0;
    int         result = 0;
};

//-------------------------------------------------------------------
// Thread Worker functions for MultiThread Request
//-------------------------------------------------------------------
void* head_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* multi_head_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* delete_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* put_head_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* put_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* list_bucket_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* check_service_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* pre_multipart_upload_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* multipart_upload_part_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* complete_multipart_upload_threadworker(S3fsCurl& s3fscurl, void* arg);
void* abort_multipart_upload_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* multipart_put_head_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* parallel_get_object_req_threadworker(S3fsCurl& s3fscurl, void* arg);
void* get_object_req_threadworker(S3fsCurl& s3fscurl, void* arg);

//-------------------------------------------------------------------
// Utility functions
//-------------------------------------------------------------------
int head_request(const std::string& strpath, headers_t& header);
int multi_head_request(const std::string& strpath, SyncFiller& syncfiller, std::mutex& thparam_lock, int& retrycount, s3obj_list_t& notfound_list, bool use_wtf8, objtype_t objtype, int& result, Semaphore& sem);
int delete_request(const std::string& strpath);
int put_head_request(const std::string& strpath, const headers_t& meta, bool is_copy);
int put_request(const std::string& strpath, const headers_t& meta, int fd, bool ahbe);
int list_bucket_request(const std::string& strpath, const std::string& query, std::string& responseBody);
int check_service_request(const std::string& strpath, bool forceNoSSE, bool support_compat_dir, long& responseCode, std::string& responseBody);
int pre_multipart_upload_request(const std::string& path, const headers_t& meta, std::string& upload_id);
int multipart_upload_part_request(const std::string& path, int upload_fd, off_t start, off_t size, int part_num, const std::string& upload_id, etagpair* petag, bool is_copy, Semaphore* psem, std::mutex* pthparam_lock, int* req_result);
int await_multipart_upload_part_request(const std::string& path, int upload_fd, off_t start, off_t size, int part_num, const std::string& upload_id, etagpair* petag, bool is_copy);
int multipart_upload_request(const std::string& path, const headers_t& meta, int upload_fd);
int mix_multipart_upload_request(const std::string& path, headers_t& meta, int upload_fd, const fdpage_list_t& mixuppages);
int complete_multipart_upload_request(const std::string& path, const std::string& upload_id, const etaglist_t& parts);
int abort_multipart_upload_request(const std::string& path, const std::string& upload_id);
int multipart_put_head_request(const std::string& strfrom, const std::string& strto, off_t size, const headers_t& meta);
int parallel_get_object_request(const std::string& path, int fd, off_t start, off_t size);
int get_object_request(const std::string& path, int fd, off_t start, off_t size);

//-------------------------------------------------------------------
// Direct Call Utility Functions
//-------------------------------------------------------------------
int get_iamv2api_token_request(const std::string& strurl, int tokenttl, const std::string& strttlhdr, std::string& token);
int get_iamrole_request(const std::string& strurl, const std::string& striamtoken, std::string& token);
int get_iamcred_request(const std::string& strurl, const std::string& striamtoken, const std::string& stribmsecret, std::string& cred);

#endif // S3FS_THREADREQS_H_

/*
* Local variables:
* tab-width: 4
* c-basic-offset: 4
* End:
* vim600: expandtab sw=4 ts=4 fdm=marker
* vim<600: expandtab sw=4 ts=4
*/
