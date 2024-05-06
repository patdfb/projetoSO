struct Tarefa{
    int estado;
    int ID;
    char argumento[300];
    int tempo;
    pid_t pid;
};

// 0-Scheduled, 1-Executing, 2-Completed