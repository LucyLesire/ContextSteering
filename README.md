# ContextSteering

The aim of this project was to explore the context steering behavior

Let's start with the basics, you have an agent and he needs to go somewhere, say a target:

![ezgif-6-6dc868dcc0ba](https://user-images.githubusercontent.com/42876636/131019116-c2cc0c91-65ae-4ed2-b648-56abd7aeaf89.gif)

Now what would happen if an obstacle is in the way of the target? 

![ezgif-6-3e06d8ecd362](https://user-images.githubusercontent.com/42876636/131020689-fa5270a8-7093-4a25-bc71-9031c9f35516.gif)

you guessed it, he just runs against the obstacle, getting stuck and having to force himself past it!

Luckily there is a way to avoid this!

Enter: Context steering!

With context steering, the agent will move around the object!

![ezgif-6-73bb2edbcbb2](https://user-images.githubusercontent.com/42876636/131025141-4c8b81f6-73d0-4b5b-b39d-d13c9382c4fe.gif)

Okay, now how do we create this?

We have a total of 5 steps
1. Fire rays in every direction around the agent
2. Figure out where we need to go
3. Remove the rays that are not important
4. Check if the rays intersect with an obstacle
5. Calculate desired outcome

Let's start one by one

1. Fire rays in every direction around the target

To do this, we will create an array of vectors called directions, this array can be as big as you like, I opted to go for 64 as a normal value, but to demonsrate, we shall use 8.

![Directions](https://user-images.githubusercontent.com/42876636/131022138-2eaeb9f6-bc6f-4ebc-b202-b53b23addf08.png)

Now we have our array filled with 8 directions around the target, looking like this

![DirectionsOnAgent](https://user-images.githubusercontent.com/42876636/131022582-09b31f26-b625-4b50-aca7-7ccf3c7abb39.png)

2. Figure out where we need to go

For out purpouse, where we need to go, is our seek, so for us that will be our go to point.

If you have a different purpouse on where the agent may need to, you will have to use that.

3. Remove the rays that are not important

Again, our agent wants to go the target of their seek, so the direction that we want to go in is towards the seek, we shall set that as our desired direction.

If our desired direction is for example up, then we don't want the agent going down, so we will have to remove all the vectors going down.

This can be done by doing a dot product on every direction and the desired direction, we shall store these dot products in an array called "Interests".

If we take the first variable in this array as the desired direction, it would look something like this

![Interests](https://user-images.githubusercontent.com/42876636/131023273-6e3cc9ba-c494-48ab-9eac-bef3b60e2e6f.png)

We mostly want to go up, but up left and up right are also okay.

4. Check if the rays intersect with an obstacle

When an obstacle appears, we will have to check for all our direction rays if they intersect with the obstacle

If they do intersect, we will add that ray to a new array called "Danger"

![image](https://user-images.githubusercontent.com/42876636/131023583-e259f2f4-19b6-4e87-a24f-2ed86734b361.png)

5. Calculate desired outcome

For our final step, we will need to add our Interests array and Danger array together, if there is danger on a direction ray, we shall remove the interest, because we don't want to go towards danger

Next we will have to add all our directions * our interests to a new Direction called "Chosen Direction"

This Chosen Direction will be the output for our agent, if there are no obstacles, it will just be towards our target and if there are objects, it will steer away from the obstacles combined with going towards the target

![ezgif-6-7b4bfd60732a](https://user-images.githubusercontent.com/42876636/131024643-e27d33dc-4d75-4d41-ab64-61a66c351df9.gif)

Our green rays are the directions rays, the turn red if the detect an obstacle and the blue ray is our Chosen Direction that the agent goes in

