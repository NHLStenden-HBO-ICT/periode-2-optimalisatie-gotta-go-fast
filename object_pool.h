#pragma once
namespace Tmpl8 {
	template<typename T>
	class ObjectPool
	{
		//this class is header only because template classes don't play nice with .cpp files
		//see https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file

	public:
		//all our shit's public because we need access to these pools
		//modify the pools in any other way but the profided methods at your own peril.
		vector<T> pool;
		
		unordered_map<T*, T*> used_items;
		unordered_map<T*, T*> free_items;


		ObjectPool(int size, T example) {
			this->size = size;
			//allocate all the memory right away
			pool.reserve(size);
			used_items.reserve(size);

			//instantiate all the items in the pool
			for (int i = 0; i < size; i++) {
				pool.emplace_back(example);
				free_items.insert({&pool[i], &pool[i] });
			}
		};
		
		/// <summary>
		/// Gets an unused item from the object pool. 
		/// It returns a pointer to that item 
		/// and keeps track internally that this item is currently in use
		/// </summary>
		/// <returns>Pointer to an unused item</returns>
		T* get() {
			mtx.lock();
			T* ptr = (*free_items.begin()).second;
			free_items.erase(ptr);
			used_items.insert({ ptr, ptr });
			mtx.unlock();
			return ptr;
		};

		/// <summary>
		/// Frees up an item that's no longer used.
		/// </summary>
		/// <param name="pointer"> points to the item that will be freed</param>
		void free(T* ptr) {
			mtx.lock();
			used_items.erase(ptr);
			free_items.insert({ ptr, ptr });
			mtx.unlock();
		};
		
		/// <summary>
		/// turn the object pool into a string for some bad debug practices ;).
		/// </summary>
		/// <returns>a string containing information about the object pool</returns>
		string to_string() {
			stringstream str;
			str << "object pool {" << endl;
			str << "  size: " << this->size << endl;
			str << "  free: " << used_items.size() << endl;
			str << "  used: " << used_items.size() << endl;
			str << "}" << endl;

			return str.str();
		}
	private:
		size_t size;
		mutex mtx;
	};
}

