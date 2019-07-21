#include <map>
#include <vector>
#include <iostream>

template<typename T>
struct m_allocator {
	using value_type = T;

	T *allocate(std::size_t n) {
		auto p = std::malloc(n * sizeof(T));
		return reinterpret_cast<T *>(p);
	}

	void deallocate(T *p, std::size_t n) {
		std::free(p);
	}

	template<typename U, typename ...Args>
	void construct(U *p, Args &&...args) {
		new(p) U(std::forward<Args>(args)...); // construct U()????
	};

	void destroy(T *p){
		p->~T();
	}

};


int main(int, char *[]) {
	auto vec = std::vector<int, m_allocator<int>>{};

	for (std::size_t i = 0; i < 1; i++) {
		vec.emplace_back(i);
		std::cout << std::endl;
	}

	for (auto i: vec) {
		std::cout << i << std::endl;
	}

	return 0;
}
