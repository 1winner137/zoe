/*******************************************************************************
*    Copyright (C) <2019-2025>, winsoft666, <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#ifndef ZOE_H_
#define ZOE_H_
#pragma once

#include <string>
#include <memory>
#include <future>
#include <map>

#ifdef ZOE_STATIC
#define ZOE_API
#else
#if defined(ZOE_EXPORTS)
#if defined(_MSC_VER)
#define ZOE_API __declspec(dllexport)
#else
#define ZOE_API
#endif
#else
#if defined(_MSC_VER)
#define ZOE_API __declspec(dllimport)
#else
#define ZOE_API
#endif
#endif
#endif

namespace zoe {
/**
 * @brief Result codes for download operations
 */
enum class ZoeResult {
  SUCCESSED = 0,                    ///< Operation completed successfully
  UNKNOWN_ERROR = 1,               ///< An unexpected error occurred
  INVALID_URL = 2,                 ///< The provided URL is invalid
  INVALID_INDEX_FORMAT = 3,        ///< The index file format is invalid
  INVALID_TARGET_FILE_PATH = 4,    ///< The target file path is invalid
  INVALID_THREAD_NUM = 5,          ///< The specified thread number is invalid
  INVALID_HASH_POLICY = 6,         ///< The hash verification policy is invalid
  INVALID_SLICE_POLICY = 7,        ///< The slice policy is invalid
  INVALID_NETWORK_CONN_TIMEOUT = 8,///< The network connection timeout is invalid
  INVALID_NETWORK_READ_TIMEOUT = 9,///< The network read timeout is invalid
  INVALID_FETCH_FILE_INFO_RETRY_TIMES = 10, ///< Invalid retry count for fetching file info
  ALREADY_DOWNLOADING = 11,        ///< A download is already in progress
  CANCELED = 12,                   ///< The download was canceled
  RENAME_TMP_FILE_FAILED = 13,     ///< Failed to rename temporary file
  OPEN_INDEX_FILE_FAILED = 14,     ///< Failed to open index file
  TMP_FILE_EXPIRED = 15,          ///< Temporary file has expired
  INIT_CURL_FAILED = 16,          ///< Failed to initialize CURL
  INIT_CURL_MULTI_FAILED = 17,     ///< Failed to initialize CURL multi handle
  SET_CURL_OPTION_FAILED = 18,     ///< Failed to set CURL option
  ADD_CURL_HANDLE_FAILED = 19,     ///< Failed to add CURL handle
  CREATE_TARGET_FILE_FAILED = 20,  ///< Failed to create target file
  CREATE_TMP_FILE_FAILED = 21,     ///< Failed to create temporary file
  OPEN_TMP_FILE_FAILED = 22,       ///< Failed to open temporary file
  URL_DIFFERENT = 23,              ///< URL differs from the one in index file
  TMP_FILE_SIZE_ERROR = 24,        ///< Temporary file size is incorrect
  TMP_FILE_CANNOT_RW = 25,         ///< Cannot read/write temporary file
  FLUSH_TMP_FILE_FAILED = 26,      ///< Failed to flush temporary file
  UPDATE_INDEX_FILE_FAILED = 27,   ///< Failed to update index file
  SLICE_DOWNLOAD_FAILED = 28,      ///< Failed to download a slice
  HASH_VERIFY_NOT_PASS = 29,       ///< Hash verification failed
  CALCULATE_HASH_FAILED = 30,      ///< Failed to calculate hash
  FETCH_FILE_INFO_FAILED = 31,     ///< Failed to fetch file information
  REDIRECT_URL_DIFFERENT = 32,     ///< Redirected URL differs from original
  NOT_CLEARLY_RESULT = 33,         ///< Result is not clearly defined
};

/**
 * @brief Current state of the download operation
 */
enum class DownloadState {
  Stopped = 0,     ///< Download is stopped
  Downloading = 1, ///< Download is in progress
  Paused = 2       ///< Download is paused
};

/**
 * @brief Policy for determining slice sizes
 */
enum class SlicePolicy {
  Auto = 0,      ///< Automatically determine slice size
  FixedSize = 1, ///< Use fixed size for slices
  FixedNum = 2   ///< Use fixed number of slices
};

/**
 * @brief Supported hash algorithms
 */
enum class HashType {
  MD5 = 0,    ///< MD5 hash algorithm
  CRC32 = 1,  ///< CRC32 hash algorithm
  SHA256 = 2  ///< SHA256 hash algorithm
};

/**
 * @brief Policy for hash verification
 */
enum class HashVerifyPolicy {
  AlwaysVerify = 0,    ///< Always verify hash
  OnlyNoFileSize = 1   ///< Verify hash only when file size is unknown
};

/**
 * @brief Policy for handling uncompleted slices
 */
enum class UncompletedSliceSavePolicy {
  AlwaysDiscard = 0,      ///< Always discard uncompleted slices
  SaveExceptFailed = 1    ///< Save uncompleted slices except failed ones
};

/**
 * @brief Event class for synchronization
 */
class ZOE_API ZoeEvent {
 public:
  ZoeEvent(bool setted = false);
  ~ZoeEvent();

  void set() noexcept;
  void unset() noexcept;
  bool isSetted() noexcept;
  bool wait(int32_t millseconds) noexcept;

 protected:
  ZoeEvent(const ZoeEvent&) = delete;
  ZoeEvent& operator=(const ZoeEvent&) = delete;
  class EventImpl;
  EventImpl* impl_;
};

typedef std::string utf8string;
typedef std::function<void(ZoeResult ret)> ResultFunctor;
typedef std::function<void(int64_t total, int64_t downloaded)> ProgressFunctor;
typedef std::function<void(int64_t byte_per_sec)> RealtimeSpeedFunctor;
typedef std::function<void(const utf8string& verbose)> VerboseOuputFunctor;
typedef std::multimap<utf8string, utf8string> HttpHeaders;

/**
 * @brief Main class for file download operations
 */
class ZOE_API Zoe {
 public:
  Zoe() noexcept;
  virtual ~Zoe() noexcept;

  static const char* GetResultString(ZoeResult enumVal);

  static void GlobalInit();
  static void GlobalUnInit();

  void setVerboseOutput(VerboseOuputFunctor verbose_functor) noexcept;

  /**
   * @brief Set the maximum number of download threads
   * @param thread_num Maximum number of threads to use
   * @return ZoeResult indicating success or failure
   * @note Set to 0 or negative to use default (1 thread)
   * @note Maximum allowed is 100 threads
   */
  ZoeResult setThreadNum(int32_t thread_num) noexcept;
  int32_t threadNum() const noexcept;

  /**
   * @brief Set the network connection timeout
   * @param milliseconds Timeout duration in milliseconds
   * @return ZoeResult indicating success or failure
   * @note Set to 0 or negative to use default (3000ms)
   * @note Only affects the connection phase
   */
  ZoeResult setNetworkConnectionTimeout(int32_t milliseconds) noexcept;
  int32_t networkConnectionTimeout() const noexcept;

  /**
   * @brief Set the number of retries for fetching file information
   * @param retry_times Number of retry attempts
   * @return ZoeResult indicating success or failure
   * @note Set to 0 or negative to use default (1 retry)
   */
  ZoeResult setRetryTimesOfFetchFileInfo(int32_t retry_times) noexcept;
  int32_t retryTimesOfFetchFileInfo() const noexcept;

  /**
   * @brief Enable/disable HEAD method for fetching file information
   * @param use_head Whether to use HEAD method
   * @return ZoeResult indicating success or failure
   */
  ZoeResult setFetchFileInfoHeadMethodEnabled(bool use_head) noexcept;
  bool fetchFileInfoHeadMethodEnabled() const noexcept;

  /**
   * @brief Set the expiration time for temporary files
   * @param seconds Time in seconds before temporary files expire
   * @return ZoeResult indicating success or failure
   * @note Default is -1 (never expire)
   */
  ZoeResult setExpiredTimeOfTmpFile(int32_t seconds) noexcept;
  int32_t expiredTimeOfTmpFile() const noexcept;

  /**
   * @brief Set the maximum download speed
   * @param byte_per_seconds Maximum speed in bytes per second
   * @return ZoeResult indicating success or failure
   * @note Set to 0 or negative to use default (-1, unlimited)
   * @note Does not affect FILE:// URLs
   */
  ZoeResult setMaxDownloadSpeed(int32_t byte_per_seconds) noexcept;
  int32_t maxDownloadSpeed() const noexcept;

  /**
   * @brief Set the minimum download speed threshold
   * @param byte_per_seconds Minimum speed in bytes per second
   * @param duration Duration in seconds to monitor speed
   * @return ZoeResult indicating success or failure
   * @note Set to 0 or negative to use default (-1, unlimited)
   */
  ZoeResult setMinDownloadSpeed(int32_t byte_per_seconds, int32_t duration) noexcept;
  int32_t minDownloadSpeed() const noexcept;
  int32_t minDownloadSpeedDuration() const noexcept;

  /**
   * @brief Set the disk cache size
   * @param cache_size Size in bytes
   * @return ZoeResult indicating success or failure
   * @note Default is 20971520 bytes (20MB)
   */
  ZoeResult setDiskCacheSize(int32_t cache_size) noexcept;
  int32_t diskCacheSize() const noexcept;

  /**
   * @brief Set the stop event for download cancellation
   * @param stop_event Pointer to the stop event
   * @return ZoeResult indicating success or failure
   * @note Download will stop when event is set
   */
  ZoeResult setStopEvent(ZoeEvent* stop_event) noexcept;
  ZoeEvent* stopEvent() noexcept;

  /**
   * @brief Enable/disable redirected URL checking
   * @param enabled Whether to check redirected URLs
   * @return ZoeResult indicating success or failure
   * @note Default is true
   */
  ZoeResult setRedirectedUrlCheckEnabled(bool enabled) noexcept;
  bool redirectedUrlCheckEnabled() const noexcept;

  /**
   * @brief Enable/disable Content-MD5 header verification
   * @param enabled Whether to verify Content-MD5
   * @return ZoeResult indicating success or failure
   * @note Default is false
   * @note Content-MD5 should be a pure MD5 string, not base64 encoded
   */
  ZoeResult setContentMd5Enabled(bool enabled) noexcept;
  bool contentMd5Enabled() const noexcept;

  /**
   * @brief Set the slice policy and value
   * @param policy The slice policy to use
   * @param policy_value The value for the policy
   * @return ZoeResult indicating success or failure
   * @note Default is fixed size of 10485760 bytes (10MB)
   */
  ZoeResult setSlicePolicy(SlicePolicy policy, int64_t policy_value) noexcept;
  void slicePolicy(SlicePolicy& policy, int64_t& policy_value) const noexcept;

  /**
   * @brief Set the hash verification policy
   * @param policy The verification policy
   * @param hash_type The type of hash to use
   * @param hash_value The expected hash value
   * @return ZoeResult indicating success or failure
   * @note Empty hash value disables verification
   */
  ZoeResult setHashVerifyPolicy(HashVerifyPolicy policy,
                                HashType hash_type,
                                const utf8string& hash_value) noexcept;
  void hashVerifyPolicy(HashVerifyPolicy& policy,
                        HashType& hash_type,
                        utf8string& hash_value) const noexcept;

  ZoeResult setHttpHeaders(const HttpHeaders& headers) noexcept;
  HttpHeaders httpHeaders() const noexcept;

  /**
   * @brief Set the proxy server
   * @param proxy Proxy server URL (e.g., "http://127.0.0.1:8888")
   * @return ZoeResult indicating success or failure
   */
  ZoeResult setProxy(const utf8string& proxy) noexcept;
  utf8string proxy() const noexcept;

  /**
   * @brief Enable/disable SSL certificate verification
   * @param enabled Whether to verify SSL certificates
   * @param ca_path Path to CA certificate bundle
   * @return ZoeResult indicating success or failure
   * @note Default is false with empty ca_path
   */
  ZoeResult setVerifyCAEnabled(bool enabled, const utf8string& ca_path) noexcept;
  bool verifyCAEnabled() const noexcept;
  utf8string caPath() const noexcept;

  /**
   * @brief Enable/disable SSL host verification
   * @param enabled Whether to verify SSL host certificates
   * @return ZoeResult indicating success or failure
   * @note Default is false
   */
  ZoeResult setVerifyHostEnabled(bool enabled) noexcept;
  bool verifyHostEnabled() const noexcept;

  /**
   * @brief Set the cookie list
   * @param cookie_list Cookie string in Netscape format
   * @return ZoeResult indicating success or failure
   * @see https://curl.se/libcurl/c/CURLOPT_COOKIELIST.html
   */
  ZoeResult setCookieList(const utf8string& cookie_list) noexcept;
  utf8string cookieList() const noexcept;

  /**
   * @brief Set the policy for uncompleted slices
   * @param policy The policy to use
   * @return ZoeResult indicating success or failure
   * @note Default is ALWAYS_DISCARD
   */
  ZoeResult setUncompletedSliceSavePolicy(UncompletedSliceSavePolicy policy) noexcept;
  UncompletedSliceSavePolicy uncompletedSliceSavePolicy() const noexcept;

  /**
   * @brief Start the download operation
   * @param url Source URL
   * @param target_file_path Local path to save the file
   * @param result_functor Callback for download completion
   * @param progress_functor Callback for download progress
   * @param realtime_speed_functor Callback for real-time speed
   * @return Future containing the download result
   */
  std::shared_future<ZoeResult> start(
      const utf8string& url,
      const utf8string& target_file_path,
      ResultFunctor result_functor,
      ProgressFunctor progress_functor,
      RealtimeSpeedFunctor realtime_speed_functor) noexcept;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  std::shared_future<ZoeResult> start(
      const std::wstring& url,
      const std::wstring& target_file_path,
      ResultFunctor result_functor,
      ProgressFunctor progress_functor,
      RealtimeSpeedFunctor realtime_speed_functor) noexcept;
#endif

  /**
   * @brief Pause the download operation
   */
  void pause() noexcept;

  /**
   * @brief Resume the download operation
   */
  void resume() noexcept;

  /**
   * @brief Stop the download operation
   * @note Will trigger CANCELED result in the result callback
   */
  void stop() noexcept;

  /**
   * @brief Get the source URL of the download
   * @return The URL being downloaded
   */
  utf8string url() const noexcept;

  /**
   * @brief Get the local file path where the download will be saved
   * @return The target file path
   */
  utf8string targetFilePath() const noexcept;

  /**
   * @brief Get the original file size from the server
   * @return File size in bytes, or -1 if unknown
   */
  int64_t originFileSize() const noexcept;

  /**
   * @brief Get the current download state
   * @return Current state of the download operation
   */
  DownloadState state() const noexcept;

  /**
   * @brief Get the future result of the download operation
   * @return Future containing the download result
   */
  std::shared_future<ZoeResult> futureResult() noexcept;

 protected:
  class ZoeImpl;
  ZoeImpl* impl_;

  Zoe(const Zoe&) = delete;
  Zoe& operator=(const Zoe&) = delete;
};
}  // namespace zoe
#endif  // !ZOE_H_