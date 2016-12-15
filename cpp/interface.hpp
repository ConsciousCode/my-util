#ifndef INTERFACE_HPP
#define INTERFACE_HPP

template<const char* name>
struct Interface {
	~Interface() {}
	
	virtual const char* __describe() {
		return name;
	}
};

//Utility macro for making interfaces, including a pprint overload
#define interface(name) \
	struct name; \
	template<typename S> \
	S pprint<S, struct name>() { return "interface " #name; } \
	struct name : public Interface<#name>

#endif

