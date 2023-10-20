# Servidor y Cliente UDP sencillos

Práctica del Tema 5 de la asignatura Arquitectura de Redes y Servicios.

Cliente y Servidor echo que intercambia mayúsculas por minúsculas y viceversa.

## Cliente

```bash
./echo-udp-client ip_servidor [-p puerto_servidor] cadena
```
La longitud de la cadena será truncada a 80 caracteres en el caso de ser más larga.

## Servidor

```bash
./echo-udp-server [-p puerto_servidor]
```

Por defecto el servidor está escuchando en el puerto 5, se necesitan permisos de root para ejecutar servicios en puertos menores a 1024.
