#ifndef HIVE_ENTROPY_NODE_H
#define HIVE_ENTROPY_NODE_H

#include <functional>
#include "Matrix.h"
#include "Message.h"
#include "CoapEndpoint.h"
#include "Row.h"
#include "Column.h"
#include "Serializer.h"

class HiveEntropyNode /*:public HiveEntropyNodeInterface*/{
    public:
        HiveEntropyNode(std::string uri);
        ~HiveEntropyNode();

        void send(Message m);

        template<typename T>
        void sendMatrixMultiplicationTask(string target, Matrix<T> a, Matrix<T> b, int insertX, int insertY, int steps, string taskId, string calculationId);
        template<typename T>
        void sendMatrixMultiplicationTask(string uri,Row<T> row, Column<T> col, string calculationId);
        template<typename T>
        void sendMatrixConvolutionTask(string target, Matrix<T> a, Matrix<T> b, string calculationId, int borderSize);

        void checkLiveness(string target);
        void queryNodeAvailability();
        void resolveNodeIdentities();

        void registerResponseHandler(coap_response_handler_t func);
        void registerMessageHandler(string uri, HttpMethod method, coap_method_handler_t func);
        void registerAsynchronousMessageHandler(string uri, HttpMethod m, Message (*func)(Message m));

        template<Message(*F)(Message)>
        void registerMessageHandler(string uri, HttpMethod method);

        void keepAlive();

    private:
        std::map<std::pair<std::string,HttpMethod>,Message (*)(Message)> asyncHandlers;
        CoapEndpoint coap;
};


//-----------------------
//Templated methods

template<Message(*F)(Message)>
void HiveEntropyNode::registerMessageHandler(string uri, HttpMethod method) {
    coap_request_t coapMethod;

    switch (method){
        case HttpMethod::GET:
            coapMethod = COAP_REQUEST_GET;
            break;
        case HttpMethod::POST:
            coapMethod = COAP_REQUEST_POST;
            break;
        case HttpMethod::PUT:
            coapMethod = COAP_REQUEST_PUT;
            break;
        case HttpMethod::DELETE:
            coapMethod = COAP_REQUEST_DELETE;
            break;
        default:
            throw "Unknown HTTP Method";
    }

    coap.addResourceHandler(uri, coapMethod, [](coap_context_t *context, coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request, coap_binary_t *token, coap_string_t *query, coap_pdu_t *response){
        Message inputMessage(session, request);
        Message output = F(inputMessage);
        output.fillResponse(resource,session,request,token,response);
    });
};

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int insertX, int insertY, int steps, string taskId, string calculationId){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of("/")!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"task/multiplication/cannon");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    std::vector<Matrix<T>> vec;
    vec.push_back(a);
    vec.push_back(b);
    m.setContent(Serializer::serialize(vec));

    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_MATRICES);
    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::ELEMENT_TYPE,std::string(typeid(T).name()));
    m.addHeader(Headers::STEPS,std::to_string(steps));
    m.addHeader(Headers::TASK_ID,taskId);
    m.addHeader(Headers::INSERT_AT_X,to_string(insertX));
    m.addHeader(Headers::INSERT_AT_Y,to_string(insertY));

    send(m);
}

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri,Row<T> row, Column<T> col, string calculationId){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of("/")!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"task/multiplication/rowcol");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    m.setContent(Serializer::serialize(row,col));

    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_ROWCOL);
    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::ELEMENT_TYPE,typeid(T).name());
    m.addHeader(Headers::INSERT_AT_X,std::to_string(row.getPosition()));
    m.addHeader(Headers::INSERT_AT_Y,std::to_string(col.getPosition()));

    send(m);
}

template<typename T>
void HiveEntropyNode::sendMatrixConvolutionTask(string uri, Matrix<T> a, Matrix<T> b, string calculationId, int borderSize){
    //LATER, BE PATIENT ;p
}

#endif