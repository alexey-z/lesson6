#include <map>
#include <vector>
#include <iostream>

template<typename T>
struct m_allocator {
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	template<typename U>
	struct rebind {
		using other = m_allocator<U>;
	};

	void *p = nullptr;
	void *c = nullptr;
	std::size_t r_s = 0;
	void *upper_bound = nullptr;
	void **ub = &upper_bound;
	std::size_t *reserved_size = &r_s;
	void **reserved = &p;
	void **current = &c;

	T *allocate(std::size_t n) {
		if (*reserved_size >= n) {
			auto ptr = reinterpret_cast<T *>(*current);
			auto c_r = (T*)*current + n;
			*current = reinterpret_cast<T *>(c_r);
			*reserved_size -= n;
			return ptr;
		} else {
			auto ptr = std::malloc(n * sizeof(T));
			return reinterpret_cast<T *>(ptr);
		}
		auto new_pt = std::malloc(n * sizeof(T));
		return reinterpret_cast<T *>(new_pt);
	}

	void reserve(std::size_t n) {
		*reserved = (T*)std::malloc(n * sizeof(T));
		*reserved_size = std::size_t(n);
		*current = *reserved;
		*ub = (T*)(*reserved)+n;
	}

	void deallocate(T *point, std::size_t n) {
		if (point < *reserved || point > *ub) {
			std::free(point);
			return;
		}
		if (point == *reserved) {
			std::free(point);
			return;
		}
	}

	template<typename U, typename ...Args>
	void construct(U *p, Args &&...args) {
		new(p) U(std::forward<Args>(args)...); // construct U()????
	};

	m_allocator() = default;
	template<typename U>
	m_allocator(const m_allocator<U> &p) {
		reserved = p.reserved;
		reserved_size = p.reserved_size;
		current = p.current;
		ub = p.ub;
	}

	~m_allocator() {  };
	void destroy(T *p){
		p->~T();
	}

};


int main(int, char *[]) {

	auto prealloc_vector = std::vector<int, m_allocator<int>>{};
	prealloc_vector.reserve(5);
	for (std::size_t i = 0; i<5; i++) {
		prealloc_vector.emplace_back(i);
	}
	for (std::size_t i = 0; i<5; i++) {
		std::cout << i << std::endl;
	}

	auto map_basic = std::map<int, int>{};
	for (std::size_t i = 0; i < 5; i++) {
		map_basic.emplace(i,i);
		std::cout << std::endl;
	}
	std::cout << "map_basic:" << std::endl;
	for (auto const& [key, val] : map_basic) {
		std::cout << key << ":" << val << std::endl;
	}

	auto map_extra = std::map<int, int, std::less<int>, m_allocator<std::pair<const int, int>>>{};
	for (std::size_t i = 0; i < 5; i++) {
		map_extra.emplace(i,i);
	}
	std::cout << "map_extra:" << std::endl; 
	for (auto const& [key, val] : map_extra) {
		std::cout << key << ":" << val << std::endl;
	}

	auto map_prealloc = std::map<int, int, std::less<int>, m_allocator<std::_Rb_tree_node<std::pair<const int, int> >>>{};
	map_prealloc.get_allocator().reserve(5);
	for (std::size_t i = 0; i < 10; i++) {
		map_prealloc.emplace(i,i);
	}

	std::cout << "map_prealloc:" << std::endl;
	for (auto const& [key, val] : map_prealloc) {
		std::cout << key << ":" << val << std::endl;
	}

	return 0;
}
