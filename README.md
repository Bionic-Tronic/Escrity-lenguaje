# Interprete - Escrity
Este es un proyecto de oseo o de practica en el lenguaje C, el cual se estara actualizando no de forma regular. El objetivo es crear un lenguaje(o la idea de uno) sencillo y facil de usar para cosas pequeñas y basicas.
## ¿Que soporta?
La verdad que no mucho. Por ahora solo se puede mostrar texto, resolver operaciones aritmeticas, crear variables, crear funciones y condicionales if y else. Pero aun hay mucho que mejorar esto solo acaba de comenzar.
## Ejemplos
Aqui estan algunas funciones que se pueden usar en el interprete:
### Mostrar texto por consola
para mostrar un texto o numero se usa la función: `sendText()`:
```
//Para imprimir un texto
sendText("Un texto\n")
//Para imprimir un número
sendText(6281)
//Para imprimir un flotante
sendText(3.1415)
//Para imprimir un caracter
sendText('E')
```
### Variables
Este interprete basico es y sera de tipado debil. Lo cual es decir que las variables pueden ser de cualquier tipo ya sean strings, char, int o floats y su creación similar a lenguajes como ruby:
```
//variable de tipo int
variable1 = 12
//variable de tipo string
variable2 = "hola"
//variable de tipo char
variable3 = 's'
//variable de tipo float
variable4 = 2.06
```
para usarlas es igual a otros lenguajes de programación:
```
sendText(variable1)//salida: 12
sendText(variable2)//salida: hola
sendText(variable3)//salida: s
sendText(variable4)//salida: 2.06
```
### Funciones
En este interprete tambien se pueden crear funciones con las palabras: `function`, `func` y `fn`(que son lo mismo no hay diferencia entre una y otra):
```
fn suma (a,b) then
   return a+b
end
func resta (a,b) then
   return a-b
end
function multiplica (a,b) then
   return a*b
end
a = suma(5,10)
b = resta(a,5)
c = multiplica(b,2)
sendText(a)
sendText(b)
sendText(c)
```
### Condicionales
Estos no pueden faltar el clasico if y else:
```
a = 12
if a > 11 then
   sendText("Es mayor que 11\n")
else
   sendText("Es menor que 11\n")
end
```
la condicional `if` no lleva parentesis y si se los agrega dara error.
Tambien esta una especie de switch
bajo en nombre de `where` (este puede cambia más adelante):
```
a = 12
where a then
   when 12:
      sendText("Vale 12\n")
   stop
   default:
      sendText("No vale 12\n")
end
```
### Bucles
Los bucles son practicamente dos hasta ahora: `for` y `while`(el for aun contiene problemas):
```
for (i = 0 at i < 10 at i = i + 1) then
     sendText("Adios\n")
end
//while
while true then
     sendText("Hola\n")
     dream 1000
end
```
### enums
Tabien hay soporte a la creación y uso de enums, donde su declaración es similar a los del lenguaje C:
```
//Declaración
enum dias then
    LUNES,
    MARTES,
    MIERCOLES,
    JUEVES,
    VIERNES,
    SABADO,
    DOMINGO
end
//uso
dia_actual = dias.LUNES
sendText(dia_actual)
```
### Objects
Objects aqui se declaran con la palabra reservada `object` o `Object`. Estos objects sus valores no son mutables en tiempo de interpretación, y al declararse dentro del `object` no pueden estar vació o esto dara error. Las propiedades de los `objects` pueden ser de tipo `int`, `char`, `string`, `float` o `funciones`:
```
//Declaración
object valores declare
     x : 12,
     y : 23,
     saludo : fn _ () then
          sendText("Saludos\n")
     end
end
//Uso
x = valores.x
y = valores.y
sendText(x)
sendText(y)
valores.saludo()
```
En esta estructura los valores deben ir siempre al inicio o antes de las funciones y las funciones despues.
### Properties
Similar a los Objects pero con modificadores de visibilidad como: `priv` y `pub`. Una de las caracteristicas es que los valores declarados pueden ser o no inicializados y darles un valor más adelanten. Si un valor fue declarado y inicializado en la estructura su valor puede cambiar más adelante intependientemente de su valor inicial(inclusive si este era una función)
En esta estructura los valores si se puede modificar despues de su declaración en la estructura,
independientemente de su valor inicial era un int, char, etc o inclusibe si era una función.
Si un valor es agregado con el modificador `priv` no se puede usar fuera de la `properties` solo dentro(de momento esto aun no se a agregado) y si es `pub` si se puede usar fuera y dentro.
(Y no no es un interprete con orientación a objectos)
```
//Declaración
properties valores then
     priv uno,
     pub dos at 2,
     pub tres,
     pub suma at (a,b) then
         return a+b
     end
end
//Uso
//Esto da error por ser un valor privado
uno = valores.uno
//Esto no dara error por ser publico
dos = valores.dos
//modifica el valor de 'tres'
valores.tres = 3
tres = valores.tres
res = valores.suma(dos,tres)
sendText(res)
```
### Arreglos
Lo arreglos en este interprete aun estan muy verdes y/o hasta muy bugeados. De momento solo hay una forma de declarlos y solo soporta valores de tipo `int` (de momento):
```
//Declaración
array numeros then
    unos is 1,
    dos is 2,
    tres is 3,
end
//uso
uno = numeros["uno"]
sendText(uno)
```
Más adelante se le agregara el soporte a poder almacenar valores de diversos tipos, tambien a la creación de matrices y tipos distintos de declarar los arreglos:
### Otras funciones
Y finalmente algunas funciones agregadas:
```
length()//devuelve la longitud de un arreglo

dream 1000//Duerme o espera 1s

getchar//Devuelve la tecla presionada

random 100//devuelve un valor
aleatorio entre 0 y el valor especificado(sino lo esta devuelva cantidades aleatorias)

gotoxy(4,5)//mueve el cursor a una posición especifica

strlen("hola")//devuelve la longitud de una cadena de texto

strcmp("a","b")//devuelve true si dos cadenas son inguales y false sino no lo son

write_file("archivo.txt","w","co tenido)//crea y escribe sobre un archivo

create_file("archivo.txt")//solo crea un archivo

remove("archivo.txt")//elimina un archivo

clear //limpia la terminal

get_size("archivo.txt")//devuleve el tamaño de un archivo.
```
## Adicional
Aun faltan muchas cosas que agregar u horres que solucionar.
Cualquiera puede modificar este codigo a su antojo o aportar tambien. Si bien el proyecto no tiene un objetivo como tal(solo para practicar) Espero te sea interesante y aprendas o practiques tu tambien!.
