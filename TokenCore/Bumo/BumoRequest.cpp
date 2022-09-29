#include "BumoRequest.h"

#include <string>

#include "mongoose.h"
#include "json/json.h"
#include "../BumoApi.h"

static int s_exit_flag = 0;
static uint64_t s_nonce = 0;
static uint64_t s_gas_price = 1000;
std::string s_str_domain_name = "http://seed1.bumo.io:16002";
std::string s_str_domain_name_testnet = "http://seed1.bumotest.io:26002";
static bool s_is_broadcast_ok = false;

static void ev_handler_get_nonce(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;

	switch (ev) {
	case MG_EV_CONNECT:
		if (*(int *)ev_data != 0) {
			fprintf(stderr, "connect() failed: %s\n", strerror(*(int *)ev_data));
		}
		break;
	case MG_EV_HTTP_REPLY: {
		nc->flags |= MG_F_CLOSE_IMMEDIATELY;
		// fwrite(hm->body.p, 1, hm->body.len, stdout);
		std::string str_response(hm->body.p, hm->body.p + hm->body.len);
		Json::Reader jReader;
		Json::Value jRoot;
		if (jReader.parse(str_response.c_str(), jRoot)) {
			if (jRoot.isObject()) {
				uint64_t err_code = jRoot["error_code"].asInt64();
				if (0 == err_code) {
					Json::Value jResult = jRoot["result"];
					s_nonce = jResult["nonce"].asInt64();
				}
			}
		}

	}
		break;
	case MG_EV_CLOSE:
		if (s_exit_flag == 0) {
			// printf("Server closed connection\n");
			s_exit_flag = 1;
		}
		break;
	default:
		break;
	}
}

uint64_t get_nonce(const std::string &str_address) {
	s_exit_flag = 0;
	struct mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);
	// std::string str_url = s_str_domain_name + "/getAccount?address=";
	std::string str_url = s_str_domain_name_testnet + "/getAccount?address=";
	str_url += str_address;
	mg_connect_http(&mgr, ev_handler_get_nonce, str_url.c_str(), NULL, NULL);

	while (s_exit_flag == 0) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return s_nonce;
}

static void ev_handler_get_gas_price(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;

	switch (ev) {
	case MG_EV_CONNECT:
		if (*(int *)ev_data != 0) {
			fprintf(stderr, "connect() failed: %s\n", strerror(*(int *)ev_data));
		}
		break;
	case MG_EV_HTTP_REPLY: {
		nc->flags |= MG_F_CLOSE_IMMEDIATELY;
		// fwrite(hm->body.p, 1, hm->body.len, stdout);
		std::string str_response(hm->body.p, hm->body.p + hm->body.len);
		Json::Reader jReader;
		Json::Value jRoot;
		if (jReader.parse(str_response.c_str(), jRoot)) {
			if (jRoot.isObject()) {
				uint64_t err_code = jRoot["error_code"].asInt64();
				if (0 == err_code) {
					Json::Value jResult = jRoot["result"];
					Json::Value jfees = jResult["fees"];
					s_gas_price = jfees["gas_price"].asInt64();
				}
			}
		}
	}
		break;
	case MG_EV_CLOSE:
		if (s_exit_flag == 0) {
			// printf("Server closed connection\n");
			s_exit_flag = 1;
		}
		break;
	default:
		break;
	}
}

uint64_t get_gas_price() {
	s_exit_flag = 0;
	struct mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);
	//std::string str_url = s_str_domain_name + "/getLedger?with_fee=true";
	std::string str_url = s_str_domain_name_testnet + "/getLedger?with_fee=true";
	mg_connect_http(&mgr, ev_handler_get_gas_price, str_url.c_str(), NULL, NULL);

	while (s_exit_flag == 0) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return s_gas_price;
}

static void ev_handler_broadcast_tx_json(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;

	switch (ev) {
	case MG_EV_CONNECT:
		if (*(int *)ev_data != 0) {
			// fprintf(stderr, "connect() failed: %s\n", strerror(*(int *)ev_data));
		}
		break;
	case MG_EV_HTTP_REPLY: {
		nc->flags |= MG_F_CLOSE_IMMEDIATELY;
		// fwrite(hm->body.p, 1, hm->body.len, stdout);
		std::string str_response(hm->body.p, hm->body.p + hm->body.len);
		Json::Reader jReader;
		Json::Value jRoot;
		if (jReader.parse(str_response.c_str(), jRoot)) {
			if (jRoot.isObject()) {
				Json::Value jResults = jRoot["results"];
				if (jResults.isArray()) {
					uint64_t err_code = jResults[0]["error_code"].asInt64();
					if (0 == err_code)
						s_is_broadcast_ok = true;
					else
						s_is_broadcast_ok = false;
				}				
			}
		}
	}
		break;
	case MG_EV_CLOSE:
		if (s_exit_flag == 0) {
			// printf("Server closed connection\n");
			s_exit_flag = 1;
		}
		break;
	default:
		break;
	}
}

bool broadcast_tx_json(const std::string &str_tx_json) {
	if ("" == str_tx_json)
		return false;

	s_exit_flag = 0;
	struct mg_mgr mgr;
	mg_mgr_init(&mgr, NULL);

	// std::string str_url = s_str_domain_name + "/submitTransaction";
	std::string str_url = s_str_domain_name_testnet + "/submitTransaction";
	mg_connect_http(&mgr, ev_handler_broadcast_tx_json, str_url.c_str(), NULL, str_tx_json.c_str());

	while (s_exit_flag == 0) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return s_is_broadcast_ok;
}
