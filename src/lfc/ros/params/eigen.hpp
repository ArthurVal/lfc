#pragma once

// SYSTEM
#include <type_traits>

// INTERNAL
#include "declare_params.hpp"
#include "raw.hpp"
#include "utils.hpp"

// EXT
// -- Eigen
#include "Eigen/Core"

// -- ROS
#include "rclcpp/node.hpp"

namespace lfc::ros {

namespace details {

/// TODO
template <class T>
struct IsEigenBase : std::is_base_of<Eigen::EigenBase<T>, T> {};

template <class T>
constexpr bool IsEigenBase_v = IsEigenBase<T>::value;

/// TODO
template <class T>
struct IsDenseBase : std::is_base_of<Eigen::DenseBase<T>, T> {};

template <class T>
constexpr bool IsDenseBase_v = IsDenseBase<T>::value;

/// TODO
template <class T>
struct IsArrayBase : std::is_base_of<Eigen::ArrayBase<T>, T> {};

template <class T>
constexpr bool IsArrayBase_v = IsArrayBase<T>::value;

/// TODO
template <class T>
struct IsMatrixBase : std::is_base_of<Eigen::MatrixBase<T>, T> {};

template <class T>
constexpr bool IsMatrixBase_v = IsMatrixBase<T>::value;

} // namespace details

template <class T>
struct ParamMatrix : public ParamWithName {

  static_assert(details::IsDenseBase_v<T> && (T::NumDimensions == 2));

  constexpr ParamMatrix() = delete;
  constexpr ParamMatrix(std::string_view name) : ParamWithName(name) {}
};

template <class T>
auto DeclareParamInto(rclcpp::Node &node, const ParamMatrix<T> &param) -> T {
  T matrix;

  auto [rows, cols] = DeclareParams(
      node,
      ParamRaw<std::int64_t>(std::string{param.Name()} + "/shape/rows")
          .ReadOnly(T::RowsAtCompileTime != Eigen::Dynamic)
          .DefaultTo(T::RowsAtCompileTime)
          .WithDescription("The number of rows of the matrix")
          .WithConstraints("Must to be >= 0"),
      ParamRaw<std::int64_t>(std::string{param.Name()} + "/shape/cols")
          .ReadOnly(T::ColsAtCompileTime != Eigen::Dynamic)
          .DefaultTo(T::ColsAtCompileTime)
          .WithDescription("The number of cols of the matrix")
          .WithConstraints("Must to be >= 0"));

  if ((T::SizeAtCompileTime == Eigen::Dynamic) && (rows >= 0 && cols >= 0)) {
    matrix.resize(rows, cols);
  }

  using value_type = std::conditional_t<std::is_integral_v<typename T::Scalar>,
                                        std::int64_t, double>;
  auto values = DeclareParams(
      node,
      ParamRaw(std::string{param.Name()} + "/values", std::vector<value_type>{})
          .WithDescription("The initial values (row major) (default to ZERO if "
                           "not provided or invalid w.r.t. the shape)"));

  if (values.size() == static_cast<std::size_t>(matrix.size())) {
    matrix = Eigen::Map<T>(values.data(), matrix.rows(), matrix.cols());
  } else {
    matrix.setZero();
  }

  return matrix;
}

template <class T>
struct ParamVector : public ParamWithName {

  static_assert(details::IsDenseBase_v<T> && (T::NumDimensions < 2));

  constexpr ParamVector() = delete;
  constexpr ParamVector(std::string_view name) : ParamWithName(name) {}
};

template <class T>
auto DeclareParamInto(rclcpp::Node &node, const ParamVector<T> &param) -> T {
  T vector;

  auto size = DeclareParams(
      node, ParamRaw<std::int64_t>(std::string{param.Name()} + "/size")
                .ReadOnly(T::SizeAtCompileTime != Eigen::Dynamic)
                .DefaultTo(T::SizeAtCompileTime)
                .WithDescription("The size of the vector")
                .WithConstraints("Must to be >= 0"));

  if ((T::SizeAtCompileTime == Eigen::Dynamic) && (size > 0)) {
    vector.resize(size);
  }

  using value_type = std::conditional_t<std::is_integral_v<typename T::Scalar>,
                                        std::int64_t, double>;

  auto values = DeclareParams(
      node,
      ParamRaw(std::string{param.Name()} + "/values", std::vector<value_type>{})
          .WithDescription("The initial values (default to ZERO if not "
                           "provided or invalid w.r.t. the size)"));

  // Induce the size from values when /size is not set
  if ((T::SizeAtCompileTime == Eigen::Dynamic) &&
      ((vector.size() == 0) && !values.empty())) {
    vector.resize(static_cast<Eigen::Index>(values.size()));
  }

  if (values.size() == static_cast<std::size_t>(vector.size())) {
    vector = Eigen::Map<T>(values.data(), vector.size());
  } else {
    vector.setZero();
  }

  return vector;
}

} // namespace lfc::ros
