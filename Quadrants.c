 /******************************************************************************
 *******************************************************************************
 This program reads a point in the Cartesian co-ordinate system and determines 
 its quadrant.
 *******************************************************************************
 ******************************************************************************/
  #include <stdio.h> /*scanf, printf definitions*/
  
  int CheckQuadrant(int xcoord, int ycoord);
  int PrintMenu();
  
  int main(void)
  { 
    int x,y;
	PrintMenu();
	scanf("%d%d",&x,&y);
	CheckQuadrant(x,y);
    return 0;
  }
  
  
  /*This function asks the user to input the x and y co-ordinates of a point 
  in the cartesian co-ordiante system*/
  
  int PrintMenu()
  {
	printf
	("Enter the x and y co-ordinates of the point seperated by a space> \n");
	printf("\t(x,y) = ");
    return 0;
  }
  
  /*This functions checks and reports which quadrant the point falls on*/
  
  int CheckQuadrant(int xcoord, int ycoord)
  { if (xcoord>0)
    {
	    if (ycoord>0)
		{
          printf("(%d,%d) is in Quadrant I\n", xcoord,ycoord);
		}
	     else if (ycoord<0)
		{
		  printf("(%d,%d) is in Quadrant IV\n", xcoord,ycoord);
		}
	}
	else if (xcoord<0)
	{
		if (ycoord>0)
	    {
	      printf("(%d,%d) is in Quadrant II\n", xcoord,ycoord);
		}
	    else if (ycoord<0)
		{
		  printf("(%d,%d) is in Quadrant III\n", xcoord,ycoord);
		}
	}
	else if ((xcoord==0) && (ycoord==0))
	{ 
        printf("(%d,%d)is at the origin\n", xcoord,ycoord);
	}
    else if ((xcoord==0) && (ycoord!=0))
	{
	    printf("(%d,%d) is on the Y-axis\n", xcoord,ycoord);
	}
	if ((ycoord==0) && (xcoord!=0))
	{ 
        printf("(%d,%d) is on the X-axis\n", xcoord,ycoord);
	}
    return 0;
  }
  
  