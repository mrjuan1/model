import bpy
from struct import pack

obj=bpy.data.objects[0]
obj.select_set(state=True)
bpy.context.view_layer.objects.active=obj

mesh=obj.data
polys=mesh.polygons

f=open(obj.name+'.bin','wb')

for poly in polys:
	for i in range(0,3):
		index=poly.vertices[i]
		vert=mesh.vertices[index].co
		f.write(pack('f',vert.x))
		f.write(pack('f',vert.y))
		f.write(pack('f',vert.z))
		index=poly.loop_indices[i]
		uv=mesh.uv_layers[0].data[index].uv
		f.write(pack('f',uv.x))
		f.write(pack('f',1.0-uv.y))

f.close()
