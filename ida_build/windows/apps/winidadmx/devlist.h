typedef struct 
	{
	int adapter;
	int lun;
	int target;
	void *pNext;
	char descr[100];
	}
	DevList;