#ifndef MEMORY_H
#define MEMORY_H

namespace sp {
    template<typename C>
    struct Deleter {
        constexpr Deleter() noexcept = default;
        template <typename D>
        Deleter(const Deleter<D> &object) noexcept {
            *this = object;
        }
        void operator()(C *object) const {
            if(object){
                delete object;
            }
        }
    };

    template<typename C>
    struct Deleter<C[]> {
        constexpr Deleter() noexcept = default;
        void operator ()(C *object) const {
            if(object){
                delete []object;
            }
        }
        template<class D> void operator ()(D *) = delete;
    };

    template<class T, class D = Deleter<T>>
    class unique_ptr
    {
    public:
        typedef T* pointer_type;
        typedef T object_type;
        typedef D deleter_type;
        
        unique_ptr() noexcept: data_ptr(nullptr), deleter {} { }
        unique_ptr(pointer_type p, D deleterr) noexcept: data_ptr(p), deleter(deleterr) { }
        unique_ptr(pointer_type p) noexcept: unique_ptr(p, Deleter<T> {}) { }

        unique_ptr(const unique_ptr<T, D> & object) = delete;
        unique_ptr& operator=(const unique_ptr<T, D> &) = delete;
        
        unique_ptr(unique_ptr<T, D> && movable_unique_ptr) noexcept: data_ptr(std::move(movable_unique_ptr.data_ptr)), 
                                                                     deleter(std::move(movable_unique_ptr.deleter)){
            movable_unique_ptr.data_ptr = nullptr;
        }
        template<typename Y, typename Z>
        unique_ptr& operator=(unique_ptr<Y, Z> &&movable_unique_ptr) noexcept{
            if(this == &movable_unique_ptr){
                return *this;
            }
            freeOldDataPtr();
            std::swap(data_ptr, movable_unique_ptr.data_ptr);
            std::swap(deleter, movable_unique_ptr.deleter);
            return *this;
        }
        ~unique_ptr() noexcept {
            freeOldDataPtr();
        }
        pointer_type get() const noexcept {
            return data_ptr ? data_ptr : nullptr;
        }
        explicit operator bool() const noexcept {
            return data_ptr ? true: false;
        }
        pointer_type release() noexcept {
            pointer_type temp = data_ptr;
            this->data_ptr = nullptr;
            return temp;
        }
        void reset(pointer_type p = pointer_type { }) noexcept {
            if(get() == p){
                return;
            }
            if(!data_ptr) {
                data_ptr = p;
                p = nullptr;
            } else {
                freeOldDataPtr();
                data_ptr = p;
                p = nullptr;
            }
        }
        void reset(std::nullptr_t) noexcept {
            freeOldDataPtr();
        }
        /* Dangerous! Use with caution */
        operator pointer_type() {
            return data_ptr;
        }

        void freeOldDataPtr() {
            deleter(data_ptr);
            data_ptr = nullptr;
        }
        
        object_type& operator*(){
            return *data_ptr;
        }
        pointer_type operator->(){
            return &this->operator*();
        }
        
    private:
        pointer_type data_ptr;
        deleter_type deleter;
    };
    
    //Not fully implemented yet
    template<typename T, typename D>
    class unique_ptr<T[], D>
    {
    public:
        typedef T object_type;
        typedef T* pointer_type;
        typedef D deleter_type;
        
        unique_ptr(pointer_type ptr, D obj): data_ptr(ptr), deleter(obj) { }
        T& get(){
            return *data_ptr;
        }
        T& operator*(){
            return *data_ptr;
        }
        T* operator->(){
            return & this->operator*();
        }
    private:
        pointer_type data_ptr;
        deleter_type deleter;
    };
}

#endif // MEMORY_H
