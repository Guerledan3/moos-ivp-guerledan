# -*- coding: utf-8 -*-

import pymoos
import time
import fonctions_coherence
import fonctions_validite


retry="FALSE"

# Cette classe nous permet de faire l'interface entre python et Moos-IvP
comms = pymoos.comms()
#Cette fonction nous permet de nous abonner aux variables dont nous avons besoin
def c():
    comms.register('VALIDE_FILE',1)
    comms.register('COHERENCE_FILE',1)
    return True
## Ces Fonctions sont celles qui gere les reactions aux nouvelles informations
def queue_callback_Validitate(msg):
    seuil_SBG=2   # Placeholder en attendant des valeurs normales
    nb_voisins=2  

    filename=Validation_des_sondes(msg.string,seuil_SBG,nb_voisins)
    comms.notify('LAST_VALIDATE',filename,pymoos.time()); 
    return True;

def queue_callback_Coherence(msg):
    msg=msg.string()
    NameFile=msg.split(";")
    resolution =1        
    seuil_MNT= 0.3 
    global retry=coherence(NameFile[0],NameFile[1],resolution,seuil_MNT) 
    comms.notify('RETOUR_QC',retry,pymoos.time()); 
    return True;


#	Main de notre programme :
#	- se connecte
#	- s'abonne aux variables interessante
#	- Repartie les informations dans differentes queues
#	- enregistre les callback par queue
#	- Publie les donnees interessantes
def main():
    
    comms.set_on_connect_callback(c); 
    comms.add_active_queue('Queue_V',queue_callback_Validitate)    
    comms.add_active_queue('Queue_C',queue_callback_Coherence) 
    comms.add_message_route_to_active_queue('Queue_V','VALIDE_FILE') 
    comms.add_message_route_to_active_queue('Queue_C','COHERENCE_FILE') 
    comms.run('localhost',9000,'pymoos')
    while True:
        time.sleep(1)
        comms.notify('HEARTBEAT','1',pymoos.time());
        
if __name__=="__main__":
        main()

        
