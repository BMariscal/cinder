// Copyright (c) Facebook, Inc. and its affiliates. (http://www.facebook.com)
#ifndef __STRICTM_ERROR_SINK_H__
#define __STRICTM_ERROR_SINK_H__

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "StrictModules/exceptions.h"
namespace strictmod {

class BaseErrorSink {
 public:
  BaseErrorSink() : errors_() {}
  virtual ~BaseErrorSink() {}

  template <typename T, typename... Args>
  void error(
      int lineno,
      int col,
      std::string filename,
      std::string scopeName,
      Args&&... args) {
    auto exc = std::make_unique<T>(
        lineno,
        col,
        std::move(filename),
        std::move(scopeName),
        std::forward<Args>(args)...);

    processError(std::move(exc));
  }

  virtual std::unique_ptr<BaseErrorSink> getNestedSink() = 0;

  bool hasError() const;

  const std::vector<std::unique_ptr<StrictModuleException>>& getErrors() const;

 protected:
  std::vector<std::unique_ptr<StrictModuleException>> errors_;

  virtual void processError(std::unique_ptr<StrictModuleException> exc) = 0;
};

/**
 * An ErrorSink raises an exception when error
 * occurs, and contain information about the location
 * of the occurence of the error, including the cause of the
 * error if there is any
 */
class ErrorSink : public BaseErrorSink {
 public:
  virtual std::unique_ptr<BaseErrorSink> getNestedSink() override;

 private:
  virtual void processError(
      std::unique_ptr<StrictModuleException> exc) override;
};

/**
 * A CollectingErrorSink stores an exception
 * but does not throw
 */
class CollectingErrorSink : public BaseErrorSink {
 public:
  virtual std::unique_ptr<BaseErrorSink> getNestedSink() override;

 private:
  virtual void processError(
      std::unique_ptr<StrictModuleException> exc) override;
};

} // namespace strictmod

#endif // __STRICTM_ERROR_SINK_H__
