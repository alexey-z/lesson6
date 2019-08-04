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
template <typename T, typename A>
class my_container {
	public:
		my_container<T, A>() {
		}
		void insert(T num) {
			if (size != 0 && (end+num) >= alloc_end) {
				auto new_alloc = allocator.allocate(sizeof(T)*size+(end-start));
				T* na = new_alloc;
				for (auto i = 0; i < size; i++) {
					na = std::move(start+i);
					na++;
				}
				allocator.deallocate(start, size);
				start = new_alloc;
				end = na;
				*end = num;
				end++;
				alloc_end = new_alloc + (sizeof(T)*size+(end-start));
				size++;
			} else {
				start = allocator.allocate(sizeof(T));
				*start = num;
				end = start+(sizeof(T));
				alloc_end = end;
				size = 1;
			}
		}

		std::size_t get_size() {
			return size;
		}

		T get_elem(int i) {
			return *reinterpret_cast<T *>(start+sizeof(T)*i);
		}

		A get_allocator() {
			return allocator;
		}

	private:
		A allocator;
		T *start=nullptr;
		T *end=nullptr;
		T *alloc_end=nullptr;
		std::size_t size=0;
};

int fact(int n) {
	int f=1;
	for (int i = 1; i <= n; i++) {
		f=f*i;
	}
	return f;
}

int main(int, char *[]) {

	auto map_basic = std::map<int, int>{};
	for (std::size_t i = 0; i < 10; i++) {
		map_basic.emplace(i,fact(i));
	}
	for (auto const& [key, val] : map_basic) {
		std::cout << key << " " << val << std::endl;
	}

	auto map_prealloc = std::map<int, int, std::less<int>, m_allocator<std::_Rb_tree_node<std::pair<const int, int> >>>{};
	map_prealloc.get_allocator().reserve(10);
	for (std::size_t i = 0; i < 10; i++) {
		map_prealloc.emplace(i,fact(i));
	}

	for (auto const& [key, val] : map_prealloc) {
		std::cout << key << " " << val << std::endl;
	}

	auto cont = my_container<int, std::allocator<int>>();
	for (std::size_t i = 0; i < 10; i++) {
		cont.insert(i);
	}
	for (std::size_t i = 0; i < 10; i++) {
		std::cout << cont.get_elem(i) << std::endl;
	}

	return 0;
}
