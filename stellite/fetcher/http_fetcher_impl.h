// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains URLFetcher, a wrapper around URLRequest that handles
// low-level details like thread safety, ref counting, and incremental buffer
// reading.  This is useful for callers who simply want to get the data from a
// URL and don't care about all the nitty-gritty details.
//
// NOTE(willchan): Only one "IO" thread is supported for URLFetcher.  This is a
// temporary situation.  We will work on allowing support for multiple "io"
// threads per process.

#ifndef STELLITE_FETCHER_HTTP_FETCHER_IMPL_H_
#define STELLITE_FETCHER_HTTP_FETCHER_IMPL_H_

#include <stdint.h>

#include <string>

#include "base/macros.h"
#include "net/base/net_export.h"
#include "net/url_request/url_fetcher.h"

namespace net {
class HttpFetcherCore;
}

namespace stellite {
class HttpFetcherDelegate;
class URLFetcherFactory;

class NET_EXPORT_PRIVATE HttpFetcherImpl : public net::URLFetcher {
 public:
  // |url| is the URL to send the request to.
  // |request_type| is the type of request to make.
  // |d| the object that will receive the callback on fetch completion.
  HttpFetcherImpl(const GURL& url,
                 RequestType request_type,
                 HttpFetcherDelegate* d,
                 bool stream_response);
  ~HttpFetcherImpl() override;

  // URLFetcher implementation:
  void SetUploadData(const std::string& upload_content_type,
                     const std::string& upload_content) override;
  void SetUploadFilePath(
      const std::string& upload_content_type,
      const base::FilePath& file_path,
      uint64_t range_offset,
      uint64_t range_length,
      scoped_refptr<base::TaskRunner> file_task_runner) override;
  void SetUploadStreamFactory(
      const std::string& upload_content_type,
      const CreateUploadStreamCallback& callback) override;
  void SetChunkedUpload(const std::string& upload_content_type) override;
  void AppendChunkToUpload(const std::string& data,
                           bool is_last_chunk) override;
  void SetLoadFlags(int load_flags) override;
  int GetLoadFlags() const override;
  void SetReferrer(const std::string& referrer) override;
  void SetReferrerPolicy(
      net::URLRequest::ReferrerPolicy referrer_policy) override;
  void SetExtraRequestHeaders(
      const std::string& extra_request_headers) override;
  void AddExtraRequestHeader(const std::string& header_line) override;
  void SetRequestContext(
      net::URLRequestContextGetter* request_context_getter) override;
  void SetInitiatorURL(const GURL& initiator) override;
  void SetURLRequestUserData(
      const void* key,
      const CreateDataCallback& create_data_callback) override;
  void SetStopOnRedirect(bool stop_on_redirect) override;
  void SetAutomaticallyRetryOn5xx(bool retry) override;
  void SetMaxRetriesOn5xx(int max_retries) override;
  int GetMaxRetriesOn5xx() const override;
  base::TimeDelta GetBackoffDelay() const override;
  void SetAutomaticallyRetryOnNetworkChanges(int max_retries) override;
  void SaveResponseToFileAtPath(
      const base::FilePath& file_path,
      scoped_refptr<base::SequencedTaskRunner> file_task_runner) override;
  void SaveResponseToTemporaryFile(
      scoped_refptr<base::SequencedTaskRunner> file_task_runner) override;
  void SaveResponseWithWriter(
      std::unique_ptr<net::URLFetcherResponseWriter> response_writer) override;
  net::HttpResponseHeaders* GetResponseHeaders() const override;
  net::HostPortPair GetSocketAddress() const override;
  bool WasFetchedViaProxy() const override;
  bool WasCached() const override;
  int64_t GetReceivedResponseContentLength() const override;
  int64_t GetTotalReceivedBytes() const override;
  void Start() override;
  const GURL& GetOriginalURL() const override;
  const GURL& GetURL() const override;
  const net::URLRequestStatus& GetStatus() const override;
  int GetResponseCode() const override;
  void ReceivedContentWasMalformed() override;
  bool GetResponseAsString(std::string* out_response_string) const override;
  bool GetResponseAsFilePath(bool take_ownership,
                             base::FilePath* out_response_path) const override;

  void Stop();

  static void CancelAll();

  static void SetIgnoreCertificateRequests(bool ignored);

  // TODO(akalin): Make these private again once URLFetcher::Create()
  // is in net/.

  static URLFetcherFactory* factory();

  // Sets the factory used by the static method Create to create a URLFetcher.
  // URLFetcher does not take ownership of |factory|. A value of NULL results
  // in a URLFetcher being created directly.
  //
  // NOTE: for safety, this should only be used through ScopedURLFetcherFactory!
  static void set_factory(URLFetcherFactory* factory);

 protected:
  // Returns the delegate.
  HttpFetcherDelegate* delegate() const;

 private:
  friend class URLFetcherTest;

  // Only used by URLFetcherTest, returns the number of URLFetcher::Core objects
  // actively running.
  static int GetNumFetcherCores();

  const scoped_refptr<net::HttpFetcherCore> core_;

  DISALLOW_COPY_AND_ASSIGN(HttpFetcherImpl);
};

}  // namespace net

#endif  // STELLITE_FETCHER_HTTP_FETCHER_IMPL_H_
