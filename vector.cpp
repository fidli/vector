#include <vector>

template <typename TType>
struct stretch_buffer
{
    // NOTE(fidli): size_t plays big role in compiler optimization
    size_t count;
    size_t capacity;
    TType * data;
    TType * data_end;

    stretch_buffer(size_t initial_size) : count(0), capacity(0), data(NULL), data_end(NULL)
    {
        reallocate(initial_size < 2 ? 2 : initial_size);
        count = initial_size;
    }

    ~stretch_buffer()
    {
        if(data)
        {
            free(data);
        }
    }

    void reallocate(size_t new_size){
        // NOTE(fidli): new_size is always > 0
        size_t byte_size = sizeof(TType)*new_size;
        data = (TType *) realloc(data, byte_size);
        capacity = new_size;
        data_end = data + new_size;
    }

    TType * begin()
    {
        return data;
    }
    
    TType * end()
    {
        return data + count;
    }

    TType size()
    {
        return count;
    }

    TType & operator[](size_t index)
    {
        return data[index];
    }

    TType & at(size_t index)
    {
        TType * target = data + index;
        if(target < data_end){
          return *target;
        } 
        throw "error"; // crash -> this adds to call time *((int *) NULL) = 0;
        return *target; // we have to return something
    }

    void push_back(TType value)
    {
        TType * target = data + count;
        if(target == data_end)
        {
            // NOTE(fidli): this is 1.5x grow, like msvc does
            size_t new_capacity = new_capacity = capacity + capacity/2;
            reallocate(new_capacity);
            // TODO(fidli): benchmark/optimize this
            *(data + count) = value;
            return;
        }
        *target = value;
    }
};

#ifndef BENCHMARK
namespace benchmark
{
using State = std::vector<int>;
template<typename T>
void DoNotOptimize(T val) { volatile T tmp = val;};
}
#define BENCHMARK(name)
#endif

static void test_creation_stretch_buffer(benchmark::State& state) {
  for (auto _ : state) {
    stretch_buffer<int> buffer(1000);
    benchmark::DoNotOptimize(buffer);
  }
}

static void test_creation_vector(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> buffer(1000);
    benchmark::DoNotOptimize(buffer);
  }
}
BENCHMARK(test_creation_vector);
BENCHMARK(test_creation_stretch_buffer);

static void test_push_back_stretch_buffer(benchmark::State& state) {
  for (auto _ : state) {
    stretch_buffer<int> buffer(0);
    for(int i = 0; i < 1000; i++)
    {
        buffer.push_back(i);
    }
    benchmark::DoNotOptimize(buffer);
  }
}

static void test_push_back_vector(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> buffer(0);
    for(int i = 0; i < 1000; i++)
    {
        buffer.push_back(i);
    }
    benchmark::DoNotOptimize(buffer);
  }
}
BENCHMARK(test_push_back_vector);
BENCHMARK(test_push_back_stretch_buffer);

static void test_acc_stretch_buffer(benchmark::State& state) {
  stretch_buffer<int> buffer(1000);
  auto size = buffer.size();
    for (auto _ : state) {
    
    for(int i = 0; i < size; i++)
    {
        buffer[i] = i*2;
    }
    benchmark::DoNotOptimize(buffer);
  }
}

static void test_acc_stretch_buffer2(benchmark::State& state) {
  stretch_buffer<int> buffer(1000);
    for (auto _ : state) {
    for(int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = i*2;
    }
    benchmark::DoNotOptimize(buffer);
  }
}

static void test_acc_vector(benchmark::State& state) {
  std::vector<int> buffer(1000);
  auto size = buffer.size();
  for (auto _ : state) {
    for(int i = 0; i < size; i++)
    {
        buffer[i] = i*2;
    }
    benchmark::DoNotOptimize(buffer);
  }
}

static void test_acc_vector2(benchmark::State& state) {
  std::vector<int> buffer(1000);
  for (auto _ : state) {
    
    for(int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = i*2;
    }
    benchmark::DoNotOptimize(buffer);
  }
}

BENCHMARK(test_acc_vector);
BENCHMARK(test_acc_vector2);
BENCHMARK(test_acc_stretch_buffer);
BENCHMARK(test_acc_stretch_buffer2);

static void test_acc2_stretch_buffer(benchmark::State& state) {
    stretch_buffer<int> buffer(1000);
    for (auto _ : state) {
    for(int & a : buffer)
    {
        a = a*2;
    }
    benchmark::DoNotOptimize(buffer);
  }
}

static void test_acc2_vector(benchmark::State& state) {
  std::vector<int> buffer(1000);
  for (auto _ : state) {
    for(int & a : buffer)
    {
        a = a*2;
    }
    benchmark::DoNotOptimize(buffer);
  }
}

BENCHMARK(test_acc2_vector);
BENCHMARK(test_acc2_stretch_buffer);

void main(){
}
