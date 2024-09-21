Parcial de cubawiki: https://www.cubawiki.com.ar/images/a/ab/SisOp_1parcial_03-10-23_enunciado.pdf

## Ejercicio 1

```python
usuariosCompraron = 0
usariosEntraronAComprar = atom(0)
permisoComprar = sem(10)
mutexComprar = sem(1)
barreraBloqueada = False
personasEnBarrera = 0

def comprasTaylorSwift():
	
	if barreraBloqueada:
		personasEnBarrera += 1
		barreraHacerCola.wait()
	
	# justo este es el espacio de codigo en donde estaran las personas esperando a comprar,
	# y la barrera de arriba la cerraremos cuando se descongestione la fila asi ninguno se cola hasta
	# que 10 de aca entraron a comprar
	
	permisoComprar.wait()

	usuariosEntraronAComprar.getAndInc()

	mutexComprar.wait()
	if usuariosCompraron == 0 and usuariosEntraronAComprar == 10:
		# si soy el primero en entrar a comprar, y ya todos los 10 entraron a esta seccion
		# abro la barrera para que hagan fila muchas personas, y asi ninguno llegó a colarse
		barreraBloqueada = False
		# dejo pasar a todos los que estaban bloqueados para que pasen a la cola de compra
		barreraHacerCola.signal(personasEnBarrera)
		perosnasEnBarrera = 0

	comprar_ticket()
	usuariosCompraron += 1

	if usuariosCompraron == 10:
		# si fui el ultimo en comprar, antes de permitir que compren 10 mas de la fila, debo
		# cerrar la barrera asi mientras aquellos entran, ninguno que no estaba en la fila se puede colar
		barreraBloqueada = True
		permisoComprar.signal(10)
		usuariosCompraron = 0
		usuariosEntraronAComprar = 0

	mutexComprar.signal()
```
