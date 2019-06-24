import io.lye;
import controles.lye;

class miclase
begin
def A,B, ready as integer;
write("Hola mundo");
enter;
read(A,B);
if(A>B)
res=A+B;
else
res=A-B;
endif;
write("Resultado",res);
end