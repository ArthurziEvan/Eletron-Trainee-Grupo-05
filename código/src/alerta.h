#include <stdio.h>
#include<stdlib.h>

float temperatura, temperaturaMaxima, temperaturaMinima, umidade, umidadeMaxima, umidadeMinima;

int verificartemperatura (float temperatura) {
    if (temperatura <= temperaturaMinima) {
        return 1; //para o alerta de temp baixas
    } 
    else if (temperatura >= temperaturaMaxima) {
        return 2; //para o alerta de temp altas
    }
    return 0; //para se ta de boa a temp
}
int verificarumidade (float umidade) {
    if (umidade <= umidadeMinima) {
        return 3; //para o alerta de umidade baixa
    }
    else if (umidade >= umidadeMaxima) {
        return 4; //para o alerta de umidade alta 
    }
    return 0; //umidade normal 
}
//obs: eu não lembrava se de fato tinha que ter o alerta de umidade e por via das duvidas eu fiz