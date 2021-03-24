#include "PoissonSampler.h"

#include <cmath>

#define DefaultPointsPerIteration 30
#define DefaultInRadius       1000

TArray<FVector> PoissonSampler::SampleSphere(int32 inSeed,FVector inCenter, float inRadius, float inMinimumDistance)
{

    // call Sample with Default
    return SampleSphere(inSeed, inCenter, inRadius, inMinimumDistance, DefaultPointsPerIteration, DefaultInRadius);
}

TArray<FVector> PoissonSampler::SampleSphere(int32 inSeed,FVector inCenter, float inRadius, float inMinimumDistance, int32 inPointsPerIteration, float inSphereRadius)
{

    // Create boundary area
    FVector outTopLeft  = inCenter - FVector(inRadius, inRadius, inRadius);
    FVector outLowerRight  =  inCenter + FVector(inRadius, inRadius, inRadius);

    // Call Sample
    return Sample(inSeed, outTopLeft, outLowerRight, inMinimumDistance, 0.0f, inPointsPerIteration, inRadius);
}


		
TArray<FVector> PoissonSampler::Sample(int32 seed, FVector inTopLeft, FVector inLowerRight, float inMinimumDistance,
                            float inRejectionDistance,  int32 inPointsPerIteration, float inSphereRadius) {
  // new settings
  PoissonSettings poissonSettings;

  poissonSettings.TopLeft = inTopLeft;
  poissonSettings.LowerRight = inLowerRight;
  poissonSettings.Dimensions = inLowerRight - inTopLeft;
  poissonSettings.Center = (inTopLeft + inLowerRight) / 2;
  poissonSettings.CellSize = inMinimumDistance / (std::sqrt(2));
  poissonSettings.MinimumDistance = inMinimumDistance;
  poissonSettings.Seed = seed;
  
  // RejectionSqDistance = rejectionDistance == null ? null : rejectionDistance * rejectionDistance
  if(inRejectionDistance!=0.0f)
  { 
    poissonSettings.RejectionSqDistance = inRejectionDistance * inRejectionDistance;
  }
  else
  {
    poissonSettings.RejectionSqDistance = 0.0f;
  }

  poissonSettings.GridWidth =
      (int32)(poissonSettings.Dimensions.X / poissonSettings.CellSize) + 1;
  poissonSettings.GridHeight =
      (int32)(poissonSettings.Dimensions.Y / poissonSettings.CellSize) + 1;
  poissonSettings.GridLength =
      (int32)(poissonSettings.Dimensions.Z / poissonSettings.CellSize) + 1;




  // random stream
  FRandomStream randomStream(poissonSettings.Seed);

  // state
  State state;

  // set size
  state.Grid.SetNum(poissonSettings.GridWidth*poissonSettings.GridLength*poissonSettings.GridHeight);

  // Add first point
  AddFirstPoint(poissonSettings, state, randomStream);

  // if active points are still available count
  while (state.ActivePoints.Num()) 
  {
    // int32 listIndex; RandomHelper.Random.Next(state.ActivePoints.Count);
    int32 listIndex = randomStream.RandRange(0, state.ActivePoints.Num()-1);

    // get point from list index
    FVector point = state.ActivePoints[listIndex];

    // if found
    bool found = false;

    // loop through iteration
    for (int32 k = 0; k <  inPointsPerIteration; k++)
      found |= AddNextPoint(point,  poissonSettings, state, randomStream);

    if (!found)
      state.ActivePoints.RemoveAt(listIndex);

    FPlatformProcess::Sleep(0.01);
  }

  int32 i=0;

  // cheat does a rotation
  for(auto Point : state.Points)
  {
    //Just get the rotation  
    FRotator rotation = Point.Rotation();

  // that should be the length
    FVector trythis = rotation.RotateVector(FVector(inSphereRadius, 0.0f, 0.0f));

    state.Points[i]=trythis;

    i++;

     FPlatformProcess::Sleep(0.01);
  }

  return state.Points;
}

// I think matches - Add first point
void PoissonSampler::AddFirstPoint(PoissonSettings inSettings, State &inState, FRandomStream & randomStream) 
{
  // do added to certain points
  bool added = false;

  while (!added) {
    // Select a spot
    float randFloat = RandomFloatFromRandomStream(randomStream);
    float xR = inSettings.TopLeft.X + (inSettings.Dimensions.X * randFloat);

    randFloat = RandomFloatFromRandomStream(randomStream);
    float yR = inSettings.TopLeft.Y + (inSettings.Dimensions.Y * randFloat);

    randFloat = RandomFloatFromRandomStream(randomStream);
    float zR = inSettings.TopLeft.Z + (inSettings.Dimensions.Z * randFloat);

    // create a point
    FVector point = FVector(xR, yR, zR);
        
    // check if distance square between points
    if ((inSettings.RejectionSqDistance != 0.0f) && (FVector::DistSquared(inSettings.Center, point) > inSettings.RejectionSqDistance))
    {
        continue;
    }

    // added
    added = true;

    // continue if added
    FVector index = Denormalize(point, inSettings.TopLeft, inSettings.CellSize);

      // use index values;
    inState.Grid[CalculateIndex(inSettings, (int32) index.X,  (int32) index.Y, (int32) index.Z)] = point;
  
    // add points
    inState.ActivePoints.Add(point);
    inState.Points.Add(point);
  }

   
}


// I think match
bool PoissonSampler::AddNextPoint(FVector inPoint, PoissonSettings inSettings,
                                   State & state, FRandomStream & randomStream) {
  bool found = false;
  FVector q = GenerateRandomAround(inPoint, inSettings.MinimumDistance, randomStream);

  // if point in topleft and lower right and notrejected by rejection or distance from centerr and point is less then
  // rejection distance  = get a index
  if ((q.X >= inSettings.TopLeft.X && q.X < inSettings.LowerRight.X) &&
      (q.Y > inSettings.TopLeft.Y && q.Y < inSettings.LowerRight.Y) &&
      (q.Z > inSettings.TopLeft.Z && q.Z < inSettings.LowerRight.Z) &&
      ( (inSettings.RejectionSqDistance == 0.0f) || (FVector::DistSquared(inSettings.Center, q) <= inSettings.RejectionSqDistance) ) 
    )
      {


    // index vector
    FVector qIndex = Denormalize(q, inSettings.TopLeft, inSettings.CellSize);

    bool tooClose = false;

    for (int32 i = (int32)FGenericPlatformMath::Max(0, (int32)qIndex.X - 2);
         i < FGenericPlatformMath::Min(inSettings.GridWidth, (int32)qIndex.X + 3) && !tooClose; i++) {
      for (int32 j = (int32)FGenericPlatformMath::Max(0,(int32) qIndex.Y - 2);
           j < FGenericPlatformMath::Min(inSettings.GridLength, (int32)qIndex.Y + 3) && !tooClose;
           j++) {
        for (int32 k = (int32)FGenericPlatformMath::Max(0, (int32)qIndex.Z - 2);
             k < FGenericPlatformMath::Min(inSettings.GridHeight, (int32)qIndex.Z + 3) && !tooClose;
             k++) {
          
          // if specific index is not 0,0,0and distance fromidex pointand q is too close return
          //true
          if (state.Grid[CalculateIndex(inSettings, i, j, k)]!=FVector(0.0f,0.0f,0.0f) &&
              FVector::Distance(
                  state.Grid[CalculateIndex(inSettings, i, j, k)], q) <
                  inSettings.MinimumDistance) {
            tooClose = true;
          }
        }
      }
    }


    // if not to close add active pons and point    
    // check if too close
    if (!tooClose) 
    {
      found = true;
      state.ActivePoints.Add(q);
      state.Points.Add(q);
      state.Grid[CalculateIndex(inSettings,(int32)qIndex.X, (int32)qIndex.Y, (int32)qIndex.Z)] = q;
    }
  }

  return found;
}

// Matches I think
float PoissonSampler::RandomFloatFromRandomStream(FRandomStream & randomStream) {
  return randomStream.GetFraction();
}

// this match up - I think
FVector PoissonSampler::GenerateRandomAround(FVector inCenter,
                                                   float inMinimumDistance, FRandomStream & randomStream)
{
  // choose amount of rotation
  float rd1 = RandomFloatFromRandomStream(randomStream);
  float rd2 = RandomFloatFromRandomStream(randomStream);
  float rd3 = RandomFloatFromRandomStream(randomStream);

  // mindset  to 2
  float radius = inMinimumDistance * (rd1 + 1.0f);

  // get y z 

  float angle1 = 2 * PI * rd2;
  float angle2 = 2 * PI * rd3;
       
  //the new point is generated around the point (x, y, z)
  float newX = radius * FGenericPlatformMath::Cos(angle1) * FGenericPlatformMath::Sin(angle2);
  float newY = radius * FGenericPlatformMath::Sin(angle1) * FGenericPlatformMath::Sin(angle2);
  float newZ = radius * FGenericPlatformMath::Cos(angle2);

  // Create a randomPoint
  FVector randomPoint =  FVector((float)(inCenter.X + newX), (float)(inCenter.Y + newY), (float)(inCenter.Z + newZ));
  
  return randomPoint;
}

// matches up - I think
FVector PoissonSampler::Denormalize(FVector inPoint, FVector inOrigin,
                                    double inCellSize) {
  return FVector((int32)((inPoint.X - inOrigin.X) / inCellSize),
                 (int32)((inPoint.Y - inOrigin.Y) / inCellSize),
                 (int32)((inPoint.Z - inOrigin.Z) / inCellSize));
}

int32 PoissonSampler::CalculateIndex(PoissonSettings inSettings, int32 i,
                                     int32 j, int32 k) {

  return i + (j * inSettings.GridWidth) +
         (k * (inSettings.GridWidth * inSettings.GridLength));
}


// do it in thesample
void PoissonSampler::ConvertXYZToSpherical(FVector &inVector, float radius)
{
  // make the point a extra data
  FVector inP = FVector(inVector.X, inVector.Y, inVector.Z);

  // This is the p symbol trigonoemtory
  //flaot pRadius = (inVector.X*inVector.X)+(inVector.Y*inVector.Y)+(inVector.Z*inVector.Z);

  // double radius
  //pRadius*=2.0f;

  // this should get the tangent
  //float tangent = inVector.Y/inVector.X;

  // this should get the arccos 
  //float acos = FGenericPlatformMath::Acos(inVector.Z/( (inVector.X*inVector.X)+(inVector.Y*inVector.Y)+(inVector.Z*inVector.Z)));

  //Just get the rotation  
  //FRotator rotation = inP.Rotation();

  // that should be the length
  //FVector trythis = rotation.RotateVector(FVector(radius, 0.0f, 0.0f));
  
  // create point
  // inVector = trythis;
}