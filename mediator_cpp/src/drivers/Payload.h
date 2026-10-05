#ifndef H_Payload
#define H_Payload

class PayloadDriver {
public:
    virtual ~PayloadDriver() = default;

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;

    virtual void testing() = 0;

    virtual void startAligning() = 0;
    virtual void stopAligning() = 0;
};

#endif
