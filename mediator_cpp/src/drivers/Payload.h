#ifndef H_Payload
#define H_Payload

class PayloadDriver {
public:
    // Virtual destructor
    virtual ~PayloadDriver() = default;

    virtual void startAligning() = 0;
    virtual void stopAligning() = 0;

    // Will add more methods as needed
};

#endif
