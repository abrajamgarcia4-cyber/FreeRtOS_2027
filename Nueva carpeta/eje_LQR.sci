// 1. Definir el espacio de estados genérico (Modificado)
A = [0 1; -2.5 -0.8];
B = [0; 4.2];
C = [1 0; 0 1]; // <-- CAMBIO AQUÍ: Ahora entrega los 2 estados
D = [0; 0];     // <-- CAMBIO AQUÍ: Dimensiones ajustadas
Planta = syslin('c', A, B, C, D);

// 2. Definir matrices de peso (3x3 para incluir el integrador)
Q = [3.7 0  0; 
     0   5  0; 
     0   0 80]; 

R = 1;

// 3. Calcular el LQR con Acción Integral (LQI)
[K_total, X] = lqi(Planta, Q, R);

// 4. Separar las ganancias para Xcos
K  = K_total(1:2);  // Ganancias de estado [K1, K2]
Ki = K_total(3);    // Ganancia integral [Ki]

disp("Ganancias de Estado (K):"); disp(K);
disp("Ganancia Integral (Ki):"); disp(Ki);
