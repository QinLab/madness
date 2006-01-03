
// Instantiations for int
template class Tensor<int>;
template class SliceTensor<int>;
template std::ostream& operator << (std::ostream& s, const Tensor<int>& t);
template Tensor<int> copy(const Tensor<int>& t);
template Tensor<int> outer(const Tensor<int>& left, const Tensor<int>& right);
template void inner_result(const Tensor<int>& left, const Tensor<int>& right,
                           long k0, long k1, Tensor<int>& result);
template Tensor<int> inner(const Tensor<int>& left, const Tensor<int>& right,
                         long k0, long k1);
template Tensor<int> transform(const Tensor<int>& t, const Tensor<int>& c);
template void fast_transform(const Tensor<int>& t, const Tensor<int>& c, Tensor<int>& result, Tensor<int>& workspace);
template Tensor< Tensor<int>::scalar_type > abs(const Tensor<int>& t);
template Tensor<int> transpose(const Tensor<int>& t);

// Instantiations for long
template class Tensor<long>;
template class SliceTensor<long>;
template std::ostream& operator << (std::ostream& s, const Tensor<long>& t);
template Tensor<long> copy(const Tensor<long>& t);
template Tensor<long> outer(const Tensor<long>& left, const Tensor<long>& right);
template void inner_result(const Tensor<long>& left, const Tensor<long>& right,
                           long k0, long k1, Tensor<long>& result);
template Tensor<long> inner(const Tensor<long>& left, const Tensor<long>& right,
                         long k0, long k1);
template Tensor<long> transform(const Tensor<long>& t, const Tensor<long>& c);
template void fast_transform(const Tensor<long>& t, const Tensor<long>& c, Tensor<long>& result, Tensor<long>& workspace);
template Tensor< Tensor<long>::scalar_type > abs(const Tensor<long>& t);
template Tensor<long> transpose(const Tensor<long>& t);

// Instantiations for double
template class Tensor<double>;
template class SliceTensor<double>;
template std::ostream& operator << (std::ostream& s, const Tensor<double>& t);
template Tensor<double> copy(const Tensor<double>& t);
template Tensor<double> outer(const Tensor<double>& left, const Tensor<double>& right);
template void inner_result(const Tensor<double>& left, const Tensor<double>& right,
                           long k0, long k1, Tensor<double>& result);
template Tensor<double> inner(const Tensor<double>& left, const Tensor<double>& right,
                         long k0, long k1);
template Tensor<double> transform(const Tensor<double>& t, const Tensor<double>& c);
template void fast_transform(const Tensor<double>& t, const Tensor<double>& c, Tensor<double>& result, Tensor<double>& workspace);
template Tensor< Tensor<double>::scalar_type > abs(const Tensor<double>& t);
template Tensor<double> transpose(const Tensor<double>& t);

// Instantiations for float
template class Tensor<float>;
template class SliceTensor<float>;
template std::ostream& operator << (std::ostream& s, const Tensor<float>& t);
template Tensor<float> copy(const Tensor<float>& t);
template Tensor<float> outer(const Tensor<float>& left, const Tensor<float>& right);
template void inner_result(const Tensor<float>& left, const Tensor<float>& right,
                           long k0, long k1, Tensor<float>& result);
template Tensor<float> inner(const Tensor<float>& left, const Tensor<float>& right,
                         long k0, long k1);
template Tensor<float> transform(const Tensor<float>& t, const Tensor<float>& c);
template void fast_transform(const Tensor<float>& t, const Tensor<float>& c, Tensor<float>& result, Tensor<float>& workspace);
template Tensor< Tensor<float>::scalar_type > abs(const Tensor<float>& t);
template Tensor<float> transpose(const Tensor<float>& t);

// Instantiations for double_complex
template class Tensor<double_complex>;
template class SliceTensor<double_complex>;
template std::ostream& operator << (std::ostream& s, const Tensor<double_complex>& t);
template Tensor<double_complex> copy(const Tensor<double_complex>& t);
template Tensor<double_complex> outer(const Tensor<double_complex>& left, const Tensor<double_complex>& right);
template void inner_result(const Tensor<double_complex>& left, const Tensor<double_complex>& right,
                           long k0, long k1, Tensor<double_complex>& result);
template Tensor<double_complex> inner(const Tensor<double_complex>& left, const Tensor<double_complex>& right,
                         long k0, long k1);
template Tensor<double_complex> transform(const Tensor<double_complex>& t, const Tensor<double_complex>& c);
template void fast_transform(const Tensor<double_complex>& t, const Tensor<double_complex>& c, Tensor<double_complex>& result, Tensor<double_complex>& workspace);
template Tensor< Tensor<double_complex>::scalar_type > abs(const Tensor<double_complex>& t);
template Tensor<double_complex> transpose(const Tensor<double_complex>& t);

// Instantiations for float_complex
template class Tensor<float_complex>;
template class SliceTensor<float_complex>;
template std::ostream& operator << (std::ostream& s, const Tensor<float_complex>& t);
template Tensor<float_complex> copy(const Tensor<float_complex>& t);
template Tensor<float_complex> outer(const Tensor<float_complex>& left, const Tensor<float_complex>& right);
template void inner_result(const Tensor<float_complex>& left, const Tensor<float_complex>& right,
                           long k0, long k1, Tensor<float_complex>& result);
template Tensor<float_complex> inner(const Tensor<float_complex>& left, const Tensor<float_complex>& right,
                         long k0, long k1);
template Tensor<float_complex> transform(const Tensor<float_complex>& t, const Tensor<float_complex>& c);
template void fast_transform(const Tensor<float_complex>& t, const Tensor<float_complex>& c, Tensor<float_complex>& result, Tensor<float_complex>& workspace);
template Tensor< Tensor<float_complex>::scalar_type > abs(const Tensor<float_complex>& t);
template Tensor<float_complex> transpose(const Tensor<float_complex>& t);

// Instantiations only for complex types

// Instantiations for double_complextemplate Tensor< Tensor<double_complex>::scalar_type > arg(const Tensor<double_complex>& t);
template Tensor< Tensor<double_complex>::scalar_type > real(const Tensor<double_complex>& t);
template Tensor< Tensor<double_complex>::scalar_type > imag(const Tensor<double_complex>& t);
template Tensor<double_complex> conj(const Tensor<double_complex>& t);
template Tensor<double_complex> conj_transpose(const Tensor<double_complex>& t);

// Instantiations for float_complextemplate Tensor< Tensor<float_complex>::scalar_type > arg(const Tensor<float_complex>& t);
template Tensor< Tensor<float_complex>::scalar_type > real(const Tensor<float_complex>& t);
template Tensor< Tensor<float_complex>::scalar_type > imag(const Tensor<float_complex>& t);
template Tensor<float_complex> conj(const Tensor<float_complex>& t);
template Tensor<float_complex> conj_transpose(const Tensor<float_complex>& t);
