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
Los bucles son practicamente dos hasta ahora: `for` y `while`(estos aun estan un poco rotos):
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
Tabiem hay soporte a la creación y uso de enums, donde su declaración es similar a los del lenguaje C:
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
Objects no en el sentido estricto de un objecto más bien algo un poco extrano. Miralo como un estructura en la que creas valores constantes y funciones:
```
//Declaración
object valores declare
     x : 12,
     y : 23,
     saludo : fn _anonima_ () then
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
En esta estructura los valores deben ir siempre al inicio o antes de las funciones y las funciones despues. La palabra `_anonima_` es temporal y se piensa cambiarla más adelante por `_`
### Properties
Similar a los Objects pero más extranos todavia. 
En esta estructura los valores si se puede modificar despues de su declaración en la estructura,
independientemente de su valor inicial era un int, char, etc o inclusibe si era una función.
Una caracteriscas de esta estructura es que hay que establecer modificadores de aceso(no esto no es un interprete con orientación a objectos)
donde `priv`(seria privado) y `pub`(publico) si este no se especifica por defecto sera `priv`.
Las properties que se declaren no estan obligadas a ser inicializada ya que sino lo son, su valor por defecto es 0. A estas properties les falta muchas implentaciones todavia(como usar sus propios valores dentro su sus propias funciones creadas):
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
Lo arreglos en este interprete estannnnn muy interesante y diferentes:
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
Por el momento esta es una unica forma de crearlos, pero se agregara tambien una forma más logica o normal:
```
//Arreglo vacio
vacio = []
//Con valores
valores = [
     uno is 1,
     dos is 2,
     a is 'a'
]
//Modificar
valores["uno"] = 2
//Agregar
valores["nuevo"] = 5
```
Y tambien de momento los arreglos no pueden cambiar su tamaño en tiempo de ejecución y ni modificar sus valores.
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
