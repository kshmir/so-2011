CityMap* readMap(char *filename)
{
        int error = 0,j,k;
        FILE *f = fopen(filename,"rb");
        CityMap *m = NULL;
        
        m = calloc(sizeof(CityMap),1);
        
        m->f = f;
        if (f != NULL)
        {
	m->size = readSize(m->f);
	
	m->map = map_init(string_comparer, string_cloner);
	m->graph = graph_init(string_comparer, string_cloner);

	j=0;
	City city;	
	while(city = readCity(m->f)!=NULL){
		j++;
		if(map_get(m->map, city->name)!=NULL) //error repeated city
			return NULL;
		graph_add_node(m->graph,city->name,city->needs)
		map_set(m->map,city->name,city->needs);

	}
	if(j!=m->size) //error wrong number of cities.
		return NULL;
	Arc arc;
	while(arc=readDistance(m->f)!=NULL){
		if(graph_get_node(m->graph, arc->from)==NULL || graph_get_node(m->graph, arc->to)==NULL)
		return NULL;
		graph_add_arc(m->graph, arc->from, arc->to, arc->weight);
        }
}

int readSize(FILE *f){
	
	char* cadena = malloc(16);
        int c = 0,error = 0,j=0;
        
        while ((c = fgetc(f)) != EOF && !error && c!=13)
        { 
		if(j=15)
			return NULL;
		cadena[j++]=c;	
	}
	cadena[j]=0;              
	return atoi(cadena);
}

City readCity(FILE *f){
	City city=calloc(1,sizeof(City));
	char* name = malloc(100);
        int c = 0,error = 0,j=0;
        
        while ((c = fgetc(f)) != EOF && !error && c!=13)
        { 
		name[j++]=c;	
	}
	name[j]=0;              
	city->name=name;	
	if(c!=13){
		free(name);
		free(city);
		return NULL;
	}
	city->needs = map_init(string_comparer, string_cloner);
	int aux;
	char* med=malloc(40);
	int ammount;
        while (aux=fscanf(f, "$40s $d\n",med,ammount) != EOF && aux != 2)
        { 	
		char*a=malloc(40);
		strcpy(a,med);
		map_set(city->needs,med,ammount);
	}
	return city;
}

Arc readDistance(FILE *f)
{
	Arc arc=calloc(1,sizeof(Arc));
	char* name = malloc(100);
        int aux = 0;
	char* from=malloc(40);
	char* to=malloc(40);	        
        aux=fscanf(f, "$40s $40s $d\n",from,to,dist);
	if(aux==EOF)
		return EOF;
	if(aux==0)
		return NULL;
	//continue
	return city;
}

