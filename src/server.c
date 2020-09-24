/*
 * server.c
 *
 *  Created on: 5 sep. 2020
 *      Author: utnso
 */


#include "server.h"

int main(void)
{
	t_log* logger;

	void iterator(char* value)
	{
		printf("%s\n", value);
	}


	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor("127.0.0.1", "4444");
	log_info(logger, "Servidor listo para recibir al cliente");

	t_contacto server;
	server.ip = "127.0.0.1";
	server.puerto = "4444";
	server.ip_lenght = strlen(server.ip) + 1;
	server.puerto_lenght = strlen(server.puerto) + 1;


	printf("cliente conectado\n");

	int cli;

	while(1)
	{
		int cliente_fd = esperar_cliente(server_fd);
		Paquete* paquetin = recibir_paquete(cliente_fd, server);
		printf("codigo recivido: %d \n", paquetin->header.tipo_mensaje);
		switch(paquetin->header.tipo_mensaje) {
			case HANDSHAKE:
				printf("Recibi handshake\n");
			break;
			case CONSULTAR_RESTAURANTES: ;
				printf("muy bien campion");
				t_list* restaurantes = list_create();
				list_add(restaurantes, (void*)"La Segunda");
				list_add(restaurantes, (void*)"Simorini");
				list_add(restaurantes, (void*)"Kentuki");

				cli = crear_conexion("127.0.0.1", "5000");
				bool status = enviar_mensaje(cliente_fd, R_CONSULTAR_RESTAURANTES, &restaurantes, server);
				printf("respuesta enviada %u", status);


				break;
			case SELECCIONAR_RESTAURANTE: ;
				t_seleccionar_restaurante* seleccionar_restaurante = (t_seleccionar_restaurante*) paquetin->mensaje;

				printf("Nombre restaurante: %s\n", seleccionar_restaurante->nombre_restaurante);
				printf("pos_x_cliente: %d\n", seleccionar_restaurante->coordenadas_cliente.pos_x);
				printf("pos_y_cliente: %d\n", seleccionar_restaurante->coordenadas_cliente.pos_y);
				printf("longitud nombre: %d\n", seleccionar_restaurante->nombre_lenght);

				char* puerto = paquetin->contacto.puerto;
				char* ip = paquetin->contacto.ip;

				bool operacion_exitosa;
				operacion_exitosa = true;

				cli = crear_conexion(ip, puerto);
				enviar_mensaje(cli, R_SELECCIONAR_RESTAURANTE, &operacion_exitosa, server);
				break;

			case CONSULTAR_PEDIDO: ;
				t_consulta_pedido* consulta_pedido = (t_consulta_pedido*) paquetin->mensaje;
				printf("Me llegaron los siguientes valores:\n");

				printf("id_p: %d\n", consulta_pedido->id_pedido);
				printf("id_r: %d\n", consulta_pedido->restaurante.identificador);
				printf("x_r: %d\n", consulta_pedido->restaurante.coordenadas.pos_x);
				printf("y_r: %d\n", consulta_pedido->restaurante.coordenadas.pos_y);
				break;

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;

		}
	}
	return EXIT_SUCCESS;
}
