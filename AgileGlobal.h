
#pragma once
#include <iostream>

struct AgileGlobal
{
	static AgileGlobal& Instance(){ static AgileGlobal obj; return obj; }
	
	AgileGlobal()
	{
		buffer_max_size = 1024 * 1024 * 10;
		buffer_mid_size = 1024 * 1024 * 1;
		buffer_min_size = 1024 * 50;
		buffer_com_size = 1024 * 5;
		//buffer_com_max_size = 1024 * 10;
		
		comm_buffer_pool_max_size = 100;
		min_buffer_pool_max_size  = 100;
		mid_buffer_pool_max_size  = 3;
		max_buffer_pool_max_size  = 2;
		
		buffer_pool_size = 10;
		buffer_string_pool_size = 10;

		sys_client_connect_timeout = 2;
		sys_client_receive_timeout = 2;
		sys_client_buffer_size = 1024 * 1000;
		
		tcp_client_connect_timeout = 2;
		tcp_client_buffer_size = 1024 * 100;
	}
	
	~AgileGlobal(){}
	
	uint32_t GetBufferComSize() const { return buffer_com_size; }
	
	uint32_t buffer_max_size;
	uint32_t buffer_mid_size;
	uint32_t buffer_min_size;
	uint32_t buffer_com_size;
	//uint32_t buffer_com_max_size;
	
	uint32_t comm_buffer_pool_max_size;
	uint32_t min_buffer_pool_max_size;
	uint32_t mid_buffer_pool_max_size;
	uint32_t max_buffer_pool_max_size;
	
	uint32_t buffer_pool_size = 10;
	uint32_t buffer_string_pool_size = 10;	

	uint32_t sys_client_connect_timeout = 2;
    uint32_t sys_client_receive_timeout = 2;
	uint32_t sys_client_buffer_size = 102400;
	
	uint32_t tcp_client_connect_timeout = 2;
	uint32_t tcp_client_buffer_size = 102400;
	
	uint32_t server_connection_pool_size = 100;
	uint32_t client_connection_pool_size = 10;

};
