# -*- coding: utf-8 -*-
"""
Created on Wed Feb 15 11:20:03 2017

@author: hureauso
"""

"""
===============================================================================
OBJECTIF 
===============================================================================
Verifier la coherence des lignes de navigation entre elles et fournir un 
fichier contenant la qualification des lignes : 0 si non traitées, 1 si valides
et 2 si non valides

===============================================================================
DESCRIPTION 
===============================================================================
3 étapes : 
    - déterminer si le 1er fichier d'entrée contient des lignes cohérentes
      si oui : test coherence général ; si non : test premier motif
      
    - création de 2 matrices : une pour chacun des deux fichiers d'entrée
    
    - détermination de la position des noeuds du motif et test du seuil sur les 
      noeuds --> test global ou test premier motif

===============================================================================    
ENTREES 
===============================================================================
    - 1 fichier contenant 2 lignes de navigation : coherentes si premier motif
      déjà fait
    
    - 1 fichier contenant 2 lignes de navigation a tester
    - la resolution de la grille de test
    - un seuil de tolérance pour la difference entre les deux grilles
    
===============================================================================    
SORTIES 
===============================================================================
    - 1 fichier x,y,z,validité

"""

#=============================================================================#
#                   Importation des bibliotheques 
#=============================================================================#
import numpy as np  
from os.path import basename, splitext

#=============================================================================#
#                   Appel des fonctions de coherence 
#=============================================================================#
def coherence(fichier_coherent,fichier_test,resolution,seuil_MNT):
    
    data_coherent=open(fichier_coherent)
    data_test=open(fichier_test)
    # !!! Ajouter test premier symbole et traitement !!! 
    # !!! Ajouter test 4 noeuds dans le doc sinon alarme et continue pour faire passer !!!
    # !!! Si les tests sont finis, virer tous les return inutiles dans les fonctions 
    
    # partie 1 : extrémités de la zone
    xmin,xmax,ymin,ymax=limites(data_coherent,data_test)
    
    # partie 2 : définitions des i et j dans les datas initiaux
    data_coherent,taille_matrice=transfo(xmin,xmax,ymin,ymax,data_coherent,resolution)
    data_test,taille_matrice=transfo(xmin,xmax,ymin,ymax,data_test,resolution)
    
    # partie 3 : matrice des lignes
    Matrice_coherent=matrice_ligne(data_coherent,taille_matrice)
    Matrice_test=matrice_ligne(data_test,taille_matrice)
    
    # partie 4 : test de la cohérence aux noeuds avec un seuil
    # on regarde si l'état de la ligne est 1 ou 0 pour savoir si on fait l'algo général de cohérence ou celui du premier motif
    if data_coherent[0,4]==1:
        diff,data_coherent,data_test=test_seuil(Matrice_coherent,Matrice_test,seuil_MNT,data_coherent,data_test)
        creation_data_sortie(data_test)
        
    if data_coherent[0,4]==0 or data_coherent[0,4]==2:
        diff,data_coherent,data_test=test_seuil_premier_motif(Matrice_coherent,Matrice_test,seuil_MNT,data_coherent,data_test)
        creation_fichier_sortie(data_coherent,fichier_coherent)
        creation_fichier_sortie(data_test,fichier_test)
    
    return fichier_coherent,fichier_test,Matrice_coherent,Matrice_test,diff 
    # return fichier_teste 


#=============================================================================#
#                   Mise en place des fonctions utilisées   
#=============================================================================# 
    
#-----------------------------------------------------------------------------#
# FONCTION qui détermine les extrémités de la zone du motif
def limites(coherent,test):
    
    # on regroupe les données des 2 fichiers sonde
    total=np.append(coherent,test,axis=0)
    # on cherche les minima et maxima
    xmin=np.min(total[:,0])
    xmax=np.max(total[:,0])
    ymin=np.min(total[:,1])
    ymax=np.max(total[:,1])
    
    return xmin,xmax,ymin,ymax

#-----------------------------------------------------------------------------#
# FONCTION affine qui donne les coordonnees dans la grille d apres les 
#          coordonnees Lambert93 et la resolution definie
def transfo(xmin,xmax,ymin,ymax,data_a_projeter,resolution):
    
    # taille des cases selon le nombre de cases que l'on veut dans la matrice (coef)   
    deltax=(xmax-xmin)/(resolution-1)
    deltay=(ymax-ymin)/(resolution-1)
    # rajout de 2 colonnes dans le data sondes pour placer i et j
    plus=np.zeros((len(data_a_projeter),2))
    data_a_projeter=np.hstack((data_a_projeter,plus))
    # i=(x-xmin)/taille case x et j=(y-ymin)/taille case y    
    data_a_projeter[:,5]=np.floor((data_a_projeter[:,0]-xmin)/deltax)
    data_a_projeter[:,6]=np.floor((data_a_projeter[:,1]-ymin)/deltay)
    # taille de la matrice en x et en y    
    taille_matrice=[np.floor((xmax-xmin)/deltax),np.floor((ymax-ymin)/deltay)]
    
    return data_a_projeter,taille_matrice

#-----------------------------------------------------------------------------#
# FONCTION qui renvoie la matrice quotient à partir des 2 matrices somme  des z 
#          et nbr valeurs dans la case
def matrice_ligne(data_projete,taille_matrice):
    
    # initialisation des matrices somme et nbr valeurs avec des zéros    
    matrice_z=np.zeros((taille_matrice[0]+1,taille_matrice[0]+1))
    matrice_nb=np.zeros((taille_matrice[1]+1,taille_matrice[1]+1))
    # on parcourt le data sondes et on défini le i,j correpondant à chaque ligne de ce data     
    for k in range(len(data_projete)):
        i=data_projete[k,5]
        j=data_projete[k,6]
        # si en i,j la matrice présente un zéro,   
        if matrice_z[i,j]==0:
            #on remplace le zéro par la valeur de z correspondant
            matrice_z[i,j]=data_projete[k,2]
            #on met 1 dans la mtrice nombre
            matrice_nb[i,j]=1
        # si en i,j il y a déjà une valeur de z,             
        else:
            #on ajoute le z actuel au z précédent
            matrice_z[i,j]=matrice_z[i,j]+data_projete[k,2]
            #on ajoute 1 au nombre précédent de valeurs
            matrice_nb[i,j]=matrice_nb[i,j]+1
    # on fait la moyenne arithmétique --> moyenne ou il y a des valeurs et nan sinon    
    matrice_quotient=np.divide(matrice_z,matrice_nb)
    
    return matrice_quotient


#------------------------------------------------------------------------------    
# FONCTION qui compare les differences aux noeuds a un seuil
def test_seuil(matrice_coherent,matrice_test,seuil,data_coherent,data_test):
    
    # différence entre la matrice cohérente et la matrice test --> valeurs au niveau des noeuds, nan sinon
    diff=matrice_coherent-matrice_test
    # récupération des positions et valeurs des noeuds 
    nds_i,nds_j=np.where(~np.isnan(diff))
    val_noeuds=diff[~np.isnan(diff)]
    
    # liste des doublons de lignes correpondantes à chaque noeuds
    # format : ligne du data coherent | ligne du data test
    lignes=np.zeros((np.size(val_noeuds),2))
    for line in range(len(val_noeuds)):
        i=nds_i[line]
        j=nds_j[line]
        for k in range(len(data_coherent)):
            if (data_coherent[k,5],data_coherent[k,6])==(i,j):
                lignes[line,0]=data_coherent[k,3]
        for l in range(len(data_test)):
            if (data_test[l,5],data_test[l,6])==(i,j):
                lignes[line,1]=data_test[l,3]
                    
        # si il y a moins de 4 noeuds, refaire les lignes
    if np.size(val_noeuds)<4:
        print('pas assez de noeuds')
    # on regroupe les valeurs par noeuds
    print(val_noeuds)
    print(lignes)
    nds_repete=np.array([[]])
    if np.size(val_noeuds)>4:
        for k in range(len(lignes)):
            for l in range(k,len(lignes)):
                if k!=l:
                    if lignes[k,0]==lignes[l,0] and lignes[k,1]==lignes[l,1]:
                        val_noeuds[k]=max(val_noeuds[k],val_noeuds[l])
                        nds_repete=np.append(nds_repete,l)
                        
        val_noeuds=np.delete(val_noeuds,nds_repete,0)
        nds_i=np.delete(nds_i,nds_repete,0)
        nds_j=np.delete(nds_j,nds_repete,0)
        lignes=np.delete(lignes,nds_repete,0)
                
    # on parcourt la liste de noeuds et on teste leur validité                   
    for line in range(len(val_noeuds)):
        # si le noeud est invalide on cherche la ligne d'appartenance et on remplace l'état par 2
        if abs(val_noeuds[line])>seuil:
            i=nds_i[line]
            j=nds_j[line]           
            for k in range(len(data_test)):
                if (data_test[k,5],data_test[k,6])==(i,j):
                    numligne=data_test[k,3]
            ind_nonvalide=np.where(data_test[:,3]==numligne) # on recupere num de ligne
            data_test[ind_nonvalide,4]=2 # Etat=ligne non valide
            print('refaire la ligne ',numligne)
        # si le noeud est valide on cherche la ligne d'appartenance et on remplace l'état par 1
        else:
            i=nds_i[line]
            j=nds_j[line]
            for k in range(len(data_test)):
                if (data_test[k,5],data_test[k,6])==(i,j):
                    numligne=data_test[k,3]
            ind_valide=np.where(data_test[:,3]==numligne)
            data_test[ind_valide,4]=1 # Etat=ligne valide
    
#------------------------------------------------------------------------------    
# FONCTION qui compare les differences aux noeuds a un seuil pour le premier motif
def test_seuil_premier_motif(Matrice_coherent,Matrice_test,seuil,data_coherent,data_test):
    # ici le data coherent ne contient pas des lignes cohérentes, il s'agit du premier data
    
    # différence entre la matrice cohérente et la matrice test --> valeurs au niveau des noeuds, nan sinon
    diff=Matrice_coherent-Matrice_test

    # récupération des positions et valeurs des noeuds 
    nds_i,nds_j=np.where(~np.isnan(diff))
    val_noeuds=abs(diff[~np.isnan(diff)])
    print(nds_i,nds_j)
    
    # liste des doublons de lignes correpondantes à chaque noeuds
    # format : ligne du data coherent | ligne du data test
    lignes=np.zeros((np.size(val_noeuds),2))
    for line in range(len(val_noeuds)):
        i=nds_i[line]
        j=nds_j[line]
        for k in range(len(data_coherent)):
            if (data_coherent[k,5],data_coherent[k,6])==(i,j):
                lignes[line,0]=data_coherent[k,3]
        for l in range(len(data_test)):
            if (data_test[l,5],data_test[l,6])==(i,j):
                lignes[line,1]=data_test[l,3]

    # si il y a moins de 4 noeuds, refaire les lignes
    if np.size(val_noeuds)<4:
        print('pas assez de noeuds')
    # on regroupe les valeurs par noeuds
    print(val_noeuds)
    print(lignes)
    # si il y a plus de 4 noeuds, on cherche les noeuds avec des lignes 
    # semblables et on prend le max de la valeur de diff au noeuds puis on 
    # supprime la deuxième ligne semblable (pour faire les tests suivants)
    
    if np.size(val_noeuds)>4:
        nds_repete=np.array([[]])
        for k in range(len(lignes)):
            for l in range(k,len(lignes)):
                if k!=l:
                    if lignes[k,0]==lignes[l,0] and lignes[k,1]==lignes[l,1]:
                        val_noeuds[k]=max(val_noeuds[k],val_noeuds[l])
                        nds_repete=np.append(nds_repete,l)
                        
        val_noeuds=np.delete(val_noeuds,nds_repete,0)
        nds_i=np.delete(nds_i,nds_repete,0)
        nds_j=np.delete(nds_j,nds_repete,0)
        lignes=np.delete(lignes,nds_repete,0)
                        
    print(lignes)
        
    invalide=np.array([[]])
    # on parcourt la liste de noeuds et on teste leur validité                   
    for line in range(len(val_noeuds)):
        # si le noeud est invalide on cherche les lignes d'appartenance et on demande à refaire les lignes
        if val_noeuds[line]>seuil:
            i=nds_i[line]
            j=nds_j[line]
            invalide=np.append(invalide,[[line]])
    
    print('noeuds invalides : ',invalide)
    s=np.size(invalide)

# si s=0, il n'y a pas de noeuds invalides
    if s==0:
        for k in range(len(data_coherent)):
                data_coherent[k,4]=1
        for l in range(len(data_test)):
                data_test[l,4]=1
        print('tous les noeuds sont valides')   
# si s=1, on refait les deux lignes du noeud invalide             
    elif s==1:        
        i=nds_i[invalide[0]]
        j=nds_j[invalide[0]]
        numligne1=lignes[invalide[0],0]
        numligne2=lignes[invalide[0],1]
        print('refaire les lignes ',numligne1,' et ',numligne2)

# si s=2, on cherche les deux doublons correspondant aux deux noeuds et on voit
# si il y a une ligne en commun                 
    elif s==2:
        line1=invalide[0]
        line2=invalide[1]
        # lignes qui appartiennent au premier data
        l1=lignes[line1,0]
        l2=lignes[line2,0]
        # lignes qui appartiennent au deuxième data
        l1p=lignes[line1,1]
        l2p=lignes[line2,1]
        
        if l1==l2:
            print('refaire la ligne ',l1)
        elif l1p==l2p:
            print('refaire la ligne ',l1p)
        else:
            print('refaire toutes les lignes')
# si s=3 ou s=4, il faut refaire toutes les lignes    
    elif s==3 or s==4:
        print('refaire toutes les lignes')
        
    return diff,data_coherent,data_test
    
##-----------------------------------------------------------------------------#
## FONCTION qui récupère le nom de la variable en string
#def string_nom_variable(variable):   
#    for name,value in globals().items() :
#        if value is variable :
#            return name
#            
#-----------------------------------------------------------------------------#
# FONCTION qui récupère le nom de la variable en string
def creation_fichier_sortie(array_final,nom):
    nom = splitext(basename(nom))[0]
    SOMME=0
    for i in range(len(array_final)):
        SOMME=SOMME+float(array_final[i,4])
    SOMME=SOMME/len(array_final)    
    if SOMME!=1:
        #os.chdir(!!!direction archive!!!)
        outfile=open(nom+'_archive.txt','w')         
    else:
        #os.chdir(!!!direction pour creation du fichier final!!!)
        outfile=open(nom+'_coherente.txt','w')  
    
    for i in range(len(array_final)):
        outfile.write("%12f %12f %7f\n"%(array_final[i,0],array_final[i,1],array_final[i,2]))

"""

Matrice pour tester le code
A=np.array([[np.nan,2,4],[1,np.nan,6]])
    
"""    
    
