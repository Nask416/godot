#pragma once

#include <atomic>

#define _NK_ATOMIC_CLASS(class_name,atomic_type)\
class class_name : public Object\
{\
	GDCLASS(class_name,Object)\
private:\
	std::atomic<atomic_type> value;\
public:\
		inline atomic_type fetch_add(const atomic_type add_value) {\
			return value.fetch_add(add_value, std::memory_order_seq_cst);\
		}\
		inline atomic_type fetch_sub(const atomic_type sub_value) {\
			return value.fetch_sub(sub_value, std::memory_order_seq_cst);\
		}\
		inline atomic_type fetch_and(const atomic_type and_value) {\
			return value.fetch_and(and_value, std::memory_order_seq_cst);\
		}\
		inline atomic_type fetch_or(const atomic_type or_value) {\
			return value.fetch_or(or_value, std::memory_order_seq_cst);\
		}\
		inline atomic_type fetch_xor(const atomic_type xor_value) {\
			return value.fetch_xor(xor_value, std::memory_order_seq_cst);\
		}\
		inline atomic_type operator++() {\
			return this->fetch_add(1);\
		}\
		inline atomic_type operator--() {\
			return this->fetch_sub(1);\
		}\
		inline atomic_type operator+=(const atomic_type add_value) {\
			return this->fetch_add(add_value);\
		}\
		inline atomic_type operator-=(const atomic_type sub_value) {\
			return this->fetch_sub(sub_value);\
		}\
		inline atomic_type operator&=(const atomic_type and_value) {\
			return this->fetch_and(and_value);\
		}\
		inline atomic_type operator|=(const atomic_type or_value) {\
			return this->fetch_or(or_value);\
		}\
		inline atomic_type operator^=(const atomic_type xor_value) {\
			return this->fetch_xor(xor_value);\
		}\
		inline atomic_type operator=(const atomic_type new_value) {\
			value.store(new_value, std::memory_order_seq_cst);\
			return new_value;\
		}\
		inline bool is_lock_free(){\
			return value.is_lock_free();\
		}\
		inline void store(const atomic_type new_value) {\
			value.store(new_value, std::memory_order_seq_cst);\
		}\
		inline atomic_type load() {\
			return value.load(std::memory_order_seq_cst);\
		}\
		inline atomic_type exchange(const atomic_type new_value) {\
			return value.exchange(new_value, std::memory_order_seq_cst);\
		}\
protected:\
	inline static void _bind_methods()\
	{\
		ClassDB::bind_method(D_METHOD("fetch_add", "add_value"), &class_name::fetch_add);\
		ClassDB::bind_method(D_METHOD("fetch_sub", "sub_value"), &class_name::fetch_sub);\
		ClassDB::bind_method(D_METHOD("fetch_and", "and_value"), &class_name::fetch_and);\
		ClassDB::bind_method(D_METHOD("fetch_or", "or_value"), &class_name::fetch_or);\
		ClassDB::bind_method(D_METHOD("fetch_xor", "xor_value"), &class_name::fetch_xor);\
		ClassDB::bind_method(D_METHOD("exchange","new_value"),&class_name::exchange);\
		ClassDB::bind_method(D_METHOD("increment"),&class_name::operator++);\
		ClassDB::bind_method(D_METHOD("decrement"),&class_name::operator--);\
		ClassDB::bind_method(D_METHOD("is_lock_free"),&class_name::is_lock_free);\
		ClassDB::bind_method(D_METHOD("store", "new_value"), &class_name::store);\
		ClassDB::bind_method(D_METHOD("load"), &class_name::load);\
	}\
};\


namespace NasK
{
	_NK_ATOMIC_CLASS(aint8,int8_t)
	_NK_ATOMIC_CLASS(auint8,uint8_t)
	_NK_ATOMIC_CLASS(aint16,int16_t)
	_NK_ATOMIC_CLASS(auint16,uint16_t)
	_NK_ATOMIC_CLASS(aint32,int32_t)
	_NK_ATOMIC_CLASS(auint32,uint32_t)
	_NK_ATOMIC_CLASS(aint64,int64_t)
	_NK_ATOMIC_CLASS(auint64,uint64_t)

	class abool : public Object
	{
		GDCLASS(abool,Object)
	private:
		std::atomic<bool> value;
	public:
			
			inline void store(const bool new_value) 
			{
				this->value.store(new_value, std::memory_order_seq_cst);
			}
			inline bool load()
			{
				return this->value.load(std::memory_order_seq_cst);
			}
			inline bool exchange(const bool new_value) 
			{
				return this->value.exchange(new_value, std::memory_order_seq_cst);
			}
			inline bool is_lock_free()
			{
				return this->value.is_lock_free();
			}
	protected:
		inline static void _bind_methods()
		{
			ClassDB::bind_method(D_METHOD("is_lock_free"),&abool::is_lock_free);
			ClassDB::bind_method(D_METHOD("exchange","new_value"),&abool::exchange);
			ClassDB::bind_method(D_METHOD("store", "new_value"), &abool::store);
			ClassDB::bind_method(D_METHOD("load"), &abool::load);
		}
	};
}