struct Tarefa{
    int estado;
    int ID;
    char argumento[300];
    int tempoEstimado;
    int tempoReal;
    int multi;
    __pid_t pid;
};

// 0-Scheduled, 1-Executing, 2-Completed