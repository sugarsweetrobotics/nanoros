#include "nanoros/rosmasterserver.h"
#include "nanoros/rosmaster.h"
#include "nanoros/rosslave.h"
#include "XmlRpc/XmlRpc.h" 
#include "XmlRpc/XmlRpcServer.h"

#include <tuple>
#include <thread>
#include <vector>
#include <queue>

#include <iostream>

using namespace ssr::nanoros;

using namespace XmlRpc;
 

class ROSMasterServerImpl;

std::optional<MasterMsg> registerService(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& service_name, const std::string& service_uri, const std::string& caller_uri);

std::optional<UnregisterInfo> unregisterService(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& service_name, const std::string& caller_uri);

std::optional<PublishersInfo> registerSubscriber(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& topic_type, const std::string& caller_uri);

std::optional<UnregisterInfo> unregisterSubscriber(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& caller_uri);

std::optional<SubscribersInfo> registerPublisher(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& topic_type, const std::string& caller_uri);

std::optional<UnregisterInfo> unregisterPublisher(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& caller_uri);

std::optional<NodeUriInfo> lookupNode(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& node_name);

std::optional<TopicTypes> getPublishedTopics(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& subgraph);

std::optional<TopicTypes>  getTopicTypes(ROSMasterServerImpl* impl, const std::string& caller_id);

std::optional<SystemState> getSystemState(ROSMasterServerImpl* impl, const std::string& caller_id);

std::optional<MasterUriInfo> getUri(ROSMasterServerImpl* impl, const std::string& caller_id);

std::optional<ServiceUriInfo> lookupService(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& srv_name);


class ROSMasterMethod : public XmlRpcServerMethod {
protected:
	ROSMasterServerImpl* impl_;
public:
	ROSMasterMethod(const std::string& name, class ROSMasterServerImpl* si);
	virtual ~ROSMasterMethod() {}

};

class RegisterService : public ROSMasterMethod {
public:
	RegisterService(class ROSMasterServerImpl* si) : ROSMasterMethod("registerServer", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string service_name = params[1];
		std::string service_uri = params[2];
		std::string caller_uri = params[3];
		auto msg = registerService(impl_, caller_id, service_name, service_uri, caller_uri);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
	}
};

class UnregisterService : public ROSMasterMethod {
public:
	UnregisterService(class ROSMasterServerImpl* si) : ROSMasterMethod("unregisterServer", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string service_name = params[1];
		std::string caller_uri = params[2];
		auto msg = unregisterService(impl_, caller_id, service_name, caller_uri);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = msg->numUnregistered;
	}
};

class RegisterPublisher : public ROSMasterMethod {
public:
	RegisterPublisher(class ROSMasterServerImpl* si) : ROSMasterMethod("registerPublisher", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string topic_name = params[1];
		std::string topic_type = params[2];
		std::string caller_uri = params[3];
		auto msg = registerPublisher(impl_, caller_id, topic_name, topic_type, caller_uri);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2].setSize(msg->subscribers.size());
		// std::cout << "MasterServer: RegisterPublisher: " << msg->subscribers.size() << std::endl;
		for (int i = 0; i < msg->subscribers.size(); i++) {
			result[2][i] = msg->subscribers[i];//  XmlRpcValue();
			//result[2][i].setSize(msg->subscribers[i].size());
			//for (int j = 0; j < msg->subscribers[i].size(); j++) {
			//	result[2][i][j] = msg->subscribers[i][j];
			//}
		}
	}
};

class UnregisterPublisher : public ROSMasterMethod {
public:
	UnregisterPublisher(class ROSMasterServerImpl* si) : ROSMasterMethod("unregisterPublisher", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string topic_name = params[1];
		std::string caller_uri = params[2];
		auto msg = unregisterPublisher(impl_, caller_id, topic_name, caller_uri);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = msg->numUnregistered;
	}
};

class RegisterSubscriber : public ROSMasterMethod {
public:
	RegisterSubscriber(class ROSMasterServerImpl* si) : ROSMasterMethod("registerSubscriber", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string topic_name = params[1];
		std::string topic_type = params[2];
		std::string caller_uri = params[3];
		auto msg = registerSubscriber(impl_, caller_id, topic_name, topic_type, caller_uri);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = XmlRpcValue();
		result[2].setSize(msg->publishers.size());
		for (int i = 0; i < msg->publishers.size(); i++) {
			result[2][i] = XmlRpcValue();
			result[2][i] = msg->publishers[i];
		}
	}
};

class UnregisterSubscriber : public ROSMasterMethod {
public:
	UnregisterSubscriber(class ROSMasterServerImpl* si) : ROSMasterMethod("unregisterSubscriber", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string topic_name = params[1];
		std::string caller_uri = params[2];
		auto msg = unregisterSubscriber(impl_, caller_id, topic_name, caller_uri);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = msg->numUnregistered;
	}
};




class LookupNode : public ROSMasterMethod {
public:
	LookupNode(class ROSMasterServerImpl* si) : ROSMasterMethod("lookupNode", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string node_name = params[1];
		auto msg = lookupNode(impl_, caller_id, node_name);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = msg->uri;
	}
};


class GetPublishedTopics : public ROSMasterMethod {
public:
	GetPublishedTopics(class ROSMasterServerImpl* si) : ROSMasterMethod("getPublishedTopics", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string subgraph = params[1];
		auto msg = getPublishedTopics(impl_, caller_id, subgraph);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = XmlRpcValue();
		result[2].setSize(msg->topicTypes.size());
		for (int i = 0; i < msg->topicTypes.size(); i++) {
			result[2][i] = XmlRpcValue();
			result[2][i][0] = msg->topicTypes[i].topicName;
			result[2][i][1] = msg->topicTypes[i].topicType;
		}
	}
};


class GetTopicTypes : public ROSMasterMethod {
public:
	GetTopicTypes(class ROSMasterServerImpl* si) : ROSMasterMethod("getTopicTypes", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		auto msg = getTopicTypes(impl_, caller_id);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = XmlRpcValue();
		result[2].setSize(msg->topicTypes.size());
		for (int i = 0; i < msg->topicTypes.size(); i++) {
			result[2][i] = XmlRpcValue();
			result[2][i][0] = msg->topicTypes[i].topicName;
			result[2][i][1] = msg->topicTypes[i].topicType;
		}
	}
};


class GetSystemState : public ROSMasterMethod {
public:
	GetSystemState(class ROSMasterServerImpl* si) : ROSMasterMethod("getSystemState", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		auto msg = getSystemState(impl_, caller_id);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = XmlRpcValue();
		result[2].setSize(3);
		result[2][0].setSize(msg->systemState.publishers.size());
		//msg->systemState.publishers.size() + msg->systemState.subscribers.size() + msg->systemState.services.size());
		for (int i = 0; i < msg->systemState.publishers.size(); i++) {
			result[2][0][i] = XmlRpcValue();
			result[2][0][i][0] = msg->systemState.publishers[i].topicName;
			result[2][0][i][1] = XmlRpcValue();
			result[2][0][i][1].setSize(msg->systemState.publishers[i].publishers.size());
			for (int j = 0; j < msg->systemState.publishers[i].publishers.size(); j++) {
				result[2][0][i][1][j] = msg->systemState.publishers[i].publishers[j];
			}
		}
		result[2][1].setSize(msg->systemState.subscribers.size());
		for (int i = 0; i < msg->systemState.subscribers.size(); i++) {
			result[2][1][i] = XmlRpcValue();
			result[2][1][i][0] = msg->systemState.subscribers[i].topicName;
			result[2][1][i][1] = XmlRpcValue();
			result[2][1][i][1].setSize(msg->systemState.subscribers[i].subscribers.size());
			for (int j = 0; j < msg->systemState.subscribers[i].subscribers.size(); j++) {
				result[2][1][i][1][j] = msg->systemState.subscribers[i].subscribers[j];
			}
		}
		result[2][2].setSize(msg->systemState.services.size());
		for (int i = 0; i < msg->systemState.services.size(); i++) {
			result[2][2][i] = XmlRpcValue();
			result[2][2][i][0] = msg->systemState.services[i].serviceName;
			result[2][2][i][1] = XmlRpcValue();
			result[2][2][i][1].setSize(msg->systemState.services[i].services.size());
			for (int j = 0; j < msg->systemState.services[i].services.size(); j++) {
				result[2][2][i][1][j] = msg->systemState.services[i].services[j];
			}
		}
	}
};


class GetUri : public ROSMasterMethod {
public:
	GetUri(class ROSMasterServerImpl* si) : ROSMasterMethod("getUri", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		auto msg = getUri(impl_, caller_id);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = msg->uri;
	}
};


class LookupService : public ROSMasterMethod {
public:
	LookupService(class ROSMasterServerImpl* si) : ROSMasterMethod("lookupService", si) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string caller_id = params[0];
		std::string service_name = params[1];
		auto msg = lookupService(impl_, caller_id, service_name);
		result[0] = msg->code;
		result[1] = msg->statusMessage;
		result[2] = msg->uri;
	}
};


struct SystemNodeInfo {
	std::string nodeName;
	std::string nodeUri;
	SystemNodeInfo(const std::string& name, const std::string& uri) : nodeName(name), nodeUri(uri) {}
	virtual ~SystemNodeInfo() {}
};

struct SystemServiceProviderNodeInfo : public SystemNodeInfo{
	std::string serviceUri;
	SystemServiceProviderNodeInfo(const std::string& name, const std::string& uri, const std::string& srv) : SystemNodeInfo(name, uri), serviceUri(srv) {}
};

struct SystemTopicsInfo {
	std::string topicName;
	std::string topicTypeName;
	std::vector<SystemNodeInfo> publishers;
	std::vector<SystemNodeInfo> subscribers;
};


struct SystemServicesInfo {
	std::string serviceName;
	std::vector<SystemServiceProviderNodeInfo> services;
};


using task_type = std::tuple<std::string, std::string>;
static const auto taskName = [](const task_type& t) { return std::get<0>(t); };
static const auto taskTopicName = [](const task_type& t) { return std::get<1>(t); };


class ROSMasterServerImpl : public ROSMasterServer {
public:

	const std::shared_ptr<XmlRpcServer> server_;
	bool exit_flag_;
	std::shared_ptr<std::thread> thread_;
	int32_t port_;

	RegisterService registerService_;
	UnregisterService unregisterService_;
	RegisterPublisher registerPublisher_;
	UnregisterPublisher unregisterPublisher_;
	RegisterSubscriber registerSubscriber_;
	UnregisterSubscriber unregisterSubscriber_;
	LookupNode lookupNode_;
	GetPublishedTopics getPublishedTopics_;
	GetTopicTypes getTopicTypes_;
	GetSystemState getSystemState_;
	GetUri getUri_;
	LookupService lookupService_;


	std::vector<SystemTopicsInfo> topics_;
	std::vector<SystemServicesInfo> services_;
public:
	ROSMasterServerImpl(const std::string& addr, const int32_t port) :
		server_(std::make_shared<XmlRpcServer>()), exit_flag_(false), port_(port),
		registerService_(this),
		unregisterService_(this),
		registerPublisher_(this),
		unregisterPublisher_(this),
		registerSubscriber_(this),
		unregisterSubscriber_(this),
		lookupNode_(this),
		getPublishedTopics_(this),
		getTopicTypes_(this),
		getSystemState_(this),
		getUri_(this),
		lookupService_(this)
	{}

	virtual ~ROSMasterServerImpl() {
		exit_flag_ = true;
		thread_->join();
	}



public:
	virtual void start(const bool blocking = true) {

		exit_flag_ = false;
		thread_ = std::make_shared<std::thread>([this]() {
			server_->bindAndListen(port_);
			server_->enableIntrospection(true);
			while(!exit_flag_) {
				server_->work(1.0);
			}
		});	 
	}

	
public:

	std::queue<task_type> task_queue_;

	virtual void spinOnce() {
		static int p = 0;
		++p;
		if (task_queue_.empty()) {
			if (p % 1000 == 0) {
				std::cout << "ROSMasterServerImpl::spinOnce(): task_queue_ is empty." << std::endl;
			}
			return;
		}

		auto task = task_queue_.front();
		task_queue_.pop();

		if (taskName(task) == "publisherUpdate") {
			std::cout << "MasterServer::spinOnce()::publisherUpdate: " << std::endl;
			for (auto topic : topics_) {
				if (topic.topicName == taskTopicName(task)) {
					std::cout << " - topic: " << topic.topicName << std::endl;
					std::vector<std::string> pubs;
					std::cout << " - publishers: " << std::endl;
					for (auto pub : topic.publishers) {
						std::cout << "     - uri: " << pub.nodeUri << std::endl;
						pubs.push_back(pub.nodeUri);
					}

					std::cout << " - subscribers:" << std::endl;
					for (auto sub : topic.subscribers) {
						std::cout << "    - uri: " << sub.nodeUri << std::endl;
						auto slave = rosslave(sub.nodeUri);
						std::cout << " - Calling publisherUpdate function" << std::endl;
						auto result = slave->publisherUpdate("rosmaster", topic.topicName, pubs);
						if (!result) {
							std::cout << " - failed." << std::endl;
						}
						else {
							if (result->code != 1) {
								std::cout << " - failed: " << result->statusMessage << std::endl;
							}
						}
					}
				}
			}
		}
	}
};



std::shared_ptr<ROSMasterServer> ssr::nanoros::masterServer(const std::optional<std::pair<std::string, int32_t>>& info) {
	if (!info) return std::make_shared<ROSMasterServer>();
	return std::make_shared<ROSMasterServerImpl>(info->first, info->second);
}


ROSMasterMethod::ROSMasterMethod(const std::string& name, class ROSMasterServerImpl* si) : XmlRpcServerMethod(name, si->server_.get()), impl_(si) {
}


// --------------------------------------------------------------------------------------------- //

std::optional<MasterMsg> registerService(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& service_name, const std::string& service_uri, const std::string& caller_uri) {
	for (auto& srv : impl->services_) {
		if (srv.serviceName == service_name) {
			for (auto& nodeInfo : srv.services) {
				if (nodeInfo.nodeName == caller_id) {
					nodeInfo.nodeUri = caller_uri;
					nodeInfo.serviceUri = service_uri;
					return MasterMsg(1, "OK");
				}
			}
			srv.services.push_back(SystemServiceProviderNodeInfo(caller_id, caller_uri, service_uri));
			return MasterMsg(1, "OK");
		}
	}
	SystemServicesInfo info;
	SystemServiceProviderNodeInfo srvInfo(caller_id, caller_uri, service_uri);
	info.serviceName = service_name;
	info.services.push_back(srvInfo);
	impl->services_.push_back(info);
	return MasterMsg(1, "OK");
}

std::optional<UnregisterInfo> unregisterService(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& service_name, const std::string& caller_uri) {
	for (auto it = impl->services_.begin(); it != impl->services_.end(); ++it) {
		auto& srv = *it;
		if (srv.serviceName == service_name) {
			for (auto it2 = srv.services.begin(); it2 != srv.services.end(); ) {
				auto& info = *it2;
				if (info.nodeName == caller_id) {
					it2 = srv.services.erase(it2);
				}
				else {
					++it2;
				}
			}
			if (srv.services.size() == 0) {
				impl->services_.erase(it);
			}
			return UnregisterInfo(1, "OK", 1);
		}
	}

	return UnregisterInfo(1, "Not found", 0);
}

std::optional<PublishersInfo> registerSubscriber(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& topic_type, const std::string& caller_uri) {
	std::cout << "MasterServer::registerSubscriber(" << caller_id << ", " << topic_name << ", " << topic_type << ", " << caller_uri << ")" << std::endl;
	for (auto& topic : impl->topics_) {
		if (topic.topicName == topic_name) {
			//if (topic.topicTypeName != topic_type) {
			//	return PublishersInfo(0, "Failed. Invalid Topic Type", {});
			//}
			for (auto& nodeInfo : topic.subscribers) {
				if (nodeInfo.nodeName == caller_id) {
					nodeInfo.nodeUri = caller_uri; // Update Node URI

					std::vector<std::string> pubInfos;
					for (auto& p : topic.publishers) {
						pubInfos.push_back(p.nodeUri);
					}
					return PublishersInfo(1, "OK", pubInfos);
				}
			}
			topic.subscribers.push_back(SystemNodeInfo(caller_id, caller_uri));
			
			std::vector<std::string> pubInfos;
			for (auto& p : topic.publishers) {
				pubInfos.push_back(p.nodeUri);
			}
			return PublishersInfo(1, "OK", pubInfos);
		}
	}

	SystemTopicsInfo info;
	SystemNodeInfo nodeInfo(caller_id, caller_uri);
	info.topicName = topic_name;
	info.topicTypeName = topic_type;
	info.subscribers.push_back(nodeInfo);
	impl->topics_.push_back(info);

	std::vector<std::string> pubInfos;
	return PublishersInfo(1, "OK", pubInfos);
}

std::optional<UnregisterInfo> unregisterSubscriber(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& caller_uri) {

	for (auto it = impl->topics_.begin(); it != impl->topics_.end(); ++it) {
		auto& topic = *it;
		if (topic.topicName == topic_name) {
			for (auto it2 = topic.subscribers.begin(); it2 != topic.subscribers.end(); ) {
				auto& info = *it2;
				if (info.nodeName == caller_id) {
					it2 = topic.subscribers.erase(it2);
				}
				else {
					++it2;
				}
			}
			if (topic.subscribers.size() == 0 && topic.publishers.size() == 0) {
				impl->topics_.erase(it);
			}
			return UnregisterInfo(1, "OK", 1);
		}
	}

	return UnregisterInfo(1, "Not found", 0);
}

std::optional<SubscribersInfo> registerPublisher(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& topic_type, const std::string& caller_uri) {
	std::cout << "MasterServer::registerPublisher(" << caller_id << ", " << topic_name << ", " << topic_type << ", " << caller_uri << ")" << std::endl;
	for (auto& topic : impl->topics_) {
		if (topic.topicName == topic_name) {
			std::cout << " - Found Topic (" << topic_name << ")" << std::endl;
			// TODO: Topic Type check must be done by roscore?
			if (topic.topicTypeName == "*") {
				std::cout << " - Previously registered topic name is *. Update to " << topic_type << std::endl;
				topic.topicTypeName = topic_type;
			} else if (topic.topicTypeName != topic_type) {
				std::cout << " - Warning: Preregistered Topic Type(" << topic.topicTypeName << ") is not same to the newly registered topic type name (" << topic_type << ")" << std::endl;
				topic.topicTypeName = topic_type;
			}

			// If master already has the same caller_id, uri must be updated by the request.
			for (auto& nodeInfo : topic.publishers) {
				if (nodeInfo.nodeName == caller_id) {
					nodeInfo.nodeUri = caller_uri; // Update Node URI
					std::cout << " - Found previously registered publisher with the same name. URI updated." << std::endl;

					std::vector<std::string> subInfos;
					for (auto& p : topic.subscribers) {
						subInfos.push_back(p.nodeUri);

					}
					// In this implementation, XmlRpc can not be done in XmlRpcServer callbacks.
					impl->task_queue_.push({ "publisherUpdate", topic_name });
					return SubscribersInfo(1, "OK", subInfos);
				}
			}

			std::cout << " - Publisher not found. New publisher." << std::endl;
			// If master does not have the caller_id, register new record.
			topic.publishers.push_back(SystemNodeInfo(caller_id, caller_uri));
			// In this implementation, XmlRpc can not be done in XmlRpcServer callbacks.
			impl->task_queue_.push({ "publisherUpdate", topic_name });

			std::vector<std::string> subInfos;
			for (auto& p : topic.subscribers) {
				std::cout << " - Subscriber(" << p.nodeName << ") found." << std::endl;
				subInfos.push_back(p.nodeUri);

			}
			return SubscribersInfo(1, "OK", subInfos);
		}
	}
	std::cout << " - Topic(" << topic_name << ") not found. New topic." << std::endl;

	SystemTopicsInfo info;
	SystemNodeInfo nodeInfo(caller_id, caller_uri);
	info.topicName = topic_name;
	info.topicTypeName = topic_type;
	info.publishers.push_back(nodeInfo);
	impl->topics_.push_back(info);

	std::vector<std::string> subInfos;
	return SubscribersInfo(1, "OK", subInfos);
}

std::optional<UnregisterInfo> unregisterPublisher(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& topic_name, const std::string& caller_uri) {

	for (auto it = impl->topics_.begin(); it != impl->topics_.end(); ++it) {
		auto& topic = *it;
		if (topic.topicName == topic_name) {
			for (auto it2 = topic.publishers.begin(); it2 != topic.publishers.end(); ) {
				auto& info = *it2;
				if (info.nodeName == caller_id) {
					std::cout << "Found caller_id:" << caller_id << std::endl;
					it2 = topic.publishers.erase(it2);

					// In this implementation, XmlRpc can not be done in XmlRpcServer callbacks.
					impl->task_queue_.push({ "publisherUpdate", topic_name });
				}
				else {
					++it2;
				}
			}
			if (topic.subscribers.size() == 0 && topic.publishers.size() == 0) {
				impl->topics_.erase(it);
			}
			return UnregisterInfo(1, "OK", 1);
		}
	}
	return UnregisterInfo(1, "Not found", 0);
}


std::optional<NodeUriInfo> lookupNode(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& node_name) {
	for (auto& topic : impl->topics_) {
		for (auto& p : topic.publishers) {
			if (p.nodeName == node_name) {
				return NodeUriInfo(1, "OK", p.nodeUri);
			}
		}
		for (auto& s : topic.subscribers) {
			if (s.nodeName == node_name) {
				return NodeUriInfo(1, "OK", s.nodeUri);
			}
		}
	}
	for (auto& service : impl->services_) {
		for (auto& s : service.services) {
			if (s.nodeName == node_name) {
				return NodeUriInfo(1, "OK", s.nodeUri);
			}
		}
	}

	return NodeUriInfo{ 0, "Failed", "" };
}

std::optional<MasterUriInfo> getUri(ROSMasterServerImpl* impl, const std::string& caller_id) {
	auto info = getROSMasterInfo();
	auto [host, port] = info.value();
	return MasterUriInfo{ 1, "OK", "http://" + host + ":" + std::to_string(port) };

}

std::optional<SystemState> getSystemState(ROSMasterServerImpl* impl, const std::string& caller_id) {
	std::vector<TopicPublishersInfo> pubs;
	std::vector<TopicSubscribersInfo> subs;
	std::vector<ServicesInfo> srvs;

	for (auto& topic : impl->topics_) {
		if (topic.publishers.size() > 0) {
			std::vector<std::string> pubnames;
			for (auto& p : topic.publishers) {
				pubnames.push_back(p.nodeUri);
			}
			pubs.emplace_back(TopicPublishersInfo{ topic.topicName, pubnames });
		}
		if (topic.subscribers.size() > 0) {
			std::vector<std::string> subnames;
			for (auto& s : topic.subscribers) {
				subnames.push_back(s.nodeUri);
			}
			subs.emplace_back(TopicSubscribersInfo{ topic.topicName, subnames });
		}
	}
	for (auto& service : impl->services_) {
		std::vector<std::string> srvnames;
		for (auto& s : service.services) {
			srvnames.push_back(s.serviceUri);
		}
		srvs.emplace_back(ServicesInfo{ service.serviceName, srvnames });
	}
	return SystemState{ 1, "OK", { pubs, subs, srvs } };
}

std::optional<TopicTypes>  getTopicTypes(ROSMasterServerImpl* impl, const std::string& caller_id) {
	std::vector<TopicTypeInfo> topicTypes;
	for (auto& topic : impl->topics_) {
		topicTypes.emplace_back(TopicTypeInfo{ topic.topicName, topic.topicTypeName });
	}
	return TopicTypes{ 1, "OK", topicTypes };
}

std::optional<TopicTypes> getPublishedTopics(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& subgraph) {
	std::vector<TopicTypeInfo> infos;
	for (auto& topic : impl->topics_) {
		if (subgraph.length() != 0 && topic.topicName.find(subgraph) != 0) continue;
		if (topic.publishers.size() > 0) {
			infos.emplace_back(TopicTypeInfo{topic.topicName, topic.topicTypeName});
		}
	}
	return TopicTypes{ 1, "OK", infos };
}

std::optional<ServiceUriInfo> lookupService(ROSMasterServerImpl* impl, const std::string& caller_id, const std::string& srv_name) {

	for (auto& service : impl->services_) {
		if (service.serviceName == srv_name) {
			if (service.services.size() > 0) {
				return ServiceUriInfo{ 1, "OK", service.services[0].serviceUri };
			}
		}
	}
	return ServiceUriInfo{0, "Failed", ""};
}



