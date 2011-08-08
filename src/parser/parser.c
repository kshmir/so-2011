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
	
	m->map = map_init(NULL, NULL); //TODO: estandarizar nombres: map_init o init_map, etc.
	m->graph = init_graph(NULL, NULL);

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
        while ((c = fgetc(f)) != EOF && !error && c!=13)
        { 
		name[j++]=c;	
	}
	return atoi(cadena);
}

