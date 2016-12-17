#ifndef INTERFACE_HPP
#define INTERFACE_HPP

struct Interface {
	~Interface() = default;
};

//Utility macro for making interfaces
#define interface(name) struct name : public Interface

#endif

