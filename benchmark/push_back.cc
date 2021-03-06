#include "benchmark/benchmark.h"
#include "varbit/vector.h"

template <typename vector_type>
uint64_t PushBack(vector_type* vector) {
  for (typename vector_type::size_type i = 0; i < Config().num_elements();
       ++i) {
    vector->push_back(i);
  }
  return vector->size();
}

template <typename vector_type>
void PushBackUsingSTL(const char* vector_name, const int max_bit_width) {
  Benchmark<vector_type*, uint64_t> benchmark_reserved(
      PushBack<vector_type>,
      vector_name,
      "PushBackReserved");
  Benchmark<vector_type*, uint64_t> benchmark_unreserved(
      PushBack<vector_type>,
      vector_name,
      "PushBackUnreserved");
  for (int bit_width = 1; bit_width <= max_bit_width; ++bit_width) {
    vector_type reserved_vector, unreserved_vector;
    reserved_vector.reserve(Config().num_elements());
    volatile uint64_t result_reserved = benchmark_reserved.run(&reserved_vector,
                                                               bit_width);
    volatile uint64_t result_unreserved = benchmark_unreserved.run(
        &unreserved_vector,
        bit_width);
    assert(result_reserved == result_unreserved);
  }
}

template <typename vector_type>
void PushBackUsingVarbit(const char* vector_name) {
  Benchmark<vector_type*, uint64_t> benchmark_reserved(
      PushBack<vector_type>,
      vector_name,
      "PushBackReserved");
  Benchmark<vector_type*, uint64_t> benchmark_unreserved(
      PushBack<vector_type>,
      vector_name,
      "PushBackUnreserved");
  for (typename varbit::bit_size_type bit_width = 1;
       bit_width <= vector_type::max_bit_width();
       ++bit_width) {
    vector_type reserved_vector(bit_width, Config().num_elements());
    vector_type unreserved_vector(bit_width);
    volatile uint64_t result_reserved = benchmark_reserved.run(&reserved_vector,
                                                               bit_width);
    volatile uint64_t result_unreserved = benchmark_unreserved.run(
        &unreserved_vector,
        bit_width);
    assert(result_reserved == result_unreserved);
  }
}

int main(int argc, char** argv) {
  Configuration config = Config(argc, argv);
  PushBackUsingVarbit<varbit::vector<uint8_t> >("varbit8");
  PushBackUsingVarbit<varbit::vector<uint16_t> >("varbit16");
  PushBackUsingVarbit<varbit::vector<uint32_t> >("varbit32");
  PushBackUsingVarbit<varbit::vector<uint64_t> >("varbit64");
  PushBackUsingSTL<std::vector<uint8_t> >("std8", 8);
  PushBackUsingSTL<std::vector<uint16_t> >("std16", 16);
  PushBackUsingSTL<std::vector<uint32_t> >("std32", 32);
  PushBackUsingSTL<std::vector<uint64_t> >("std64", 64);
  return 0;
}
