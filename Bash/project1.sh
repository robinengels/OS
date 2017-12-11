#!/bin/bash
#Robin Engels n°matricule : 000426221
find_double() #Param : Liste des fichiers, MD5 recherché, Main Directory,Chemin jusqu'au fichier, nom du fichier, taille du fichier
{
  to_print=" " #Variable contenant les chemins des fichiers identiques
  found="0" #Indique si on a trouvé un double ( 1 = True, O = False)
  for x in $1 #On teste chaque fichier/dossier
  do

    if [ ${x:0:1} == "." ] #Si c'est un dossier
    then

      if [ ${x#./} == ".:" ]
      then
        current_dir=$3 #Si on est sur le main directory on initialise current_dir
      else

        current_dir=$3/${x#./} #On change le répertoire actuel
        current_dir=${current_dir%:}
      fi


    elif [ -f $current_dir/$x ] #Si c'est un fichier
    then
      current_filesize=$(stat -c%s "$current_dir/$x") #On calcul sa taille
      if [ $current_filesize == $6 ] #Si c'est la même taille
      then
        current_md5=`md5sum $current_dir/$x | cut -d' ' -f1` #On calcul le md5
        if [ $current_md5 == $2 ] #Si c'est les même
        then
          fichier=${fichier/$x/""} #On retire ce fichier de la liste a traiter
          to_print+="$current_dir/$x ," #On l'ajoute au fichier a imprimer

        fi
      fi
    fi



  done

  if [ $to_print != " " ] && [ $to_print != " $4 ," ] #Si on a trouvé des doublons
  then
    to_print=${to_print%,} #On retire la dernière virgule
    echo $to_print #On affiche le résultat
  fi

}

main(){
fichier=`ls -R` #On liste tout les fichier

for x in $fichier #On parcourt tout le dossier
do
  if [ ${x:0:1} == "." ] #Si on est sur un sous dossier
  then

    if [ ${x#./} == ".:" ] #Si on est sur le dossier courant
    then
      dir="`pwd`" #Dir est le répertoire qu'on traite

    else
      dir=`pwd`/${x#./} #On change de répertoire
      dir=${dir%:} #On retire les ":" a la fin
    fi

  elif [ -f $dir/$x ] #Si c'est un fichier
  then

  md5=`md5sum $dir/$x | cut -d' ' -f1` #On calcul le md5
  filesize=$(stat -c%s "$dir/$x") #On calcul la taille du fichier
  find_double "$fichier" "$md5" "`pwd`" "$dir/$x" "$x" "$filesize" #On apelle find_double

  fi
done
}

cd $1 #On va dans le répertoire voulu
IFS=$'\n' #On définit le séparateur
main #On lance la fonction main sans paramètre
