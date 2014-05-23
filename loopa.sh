while true
do
        ./mx null count=1 "pft=s1:=(date),s1/,f(seconds(s1),1,0)/" 
        sleep 1
done

