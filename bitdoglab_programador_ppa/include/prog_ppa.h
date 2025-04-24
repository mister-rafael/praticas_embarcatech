#ifndef PROG_PPA_H
#define PROG_PPA_H

void enviar_comando_uart(const char *comando);
void atualizar_display(const char *linha1, const char *linha2);
void iniciar_uart(void);
void iniciar_display(void);
void checar_botoes(void);

#endif