USTRUCT()
struct UPoissonSettings {
public:
  FVector3 TopLeft, LowerRight, Center;
  FVector Dimensions;
  float RejectionSqDistance;
  float MinimumDistance;
  float CellSize;
  int GridWidth, GridHeight, GridLength;
}

UCLASS() 
class State {
    public:
}

List<FVector> PoissonSampler::SampleSphere(FVector inCenter, float inRadius, float inMinimumDistance) 
{
  return SampleSphere(inCenter, inRadius, inMinimumDistance, DefaultPointsPerIteration);
}

List<FVector> PoissonSampler::SampleSphere(FVector inCenter, float inRadius, float inMinimumDistance, int32 inPointsPerIteration) 
{
  return Sample(center, inRadius, float inMinimumDistance, inPointsPerIteration);
}

void PoissonSampler::Sample(int seed, FVector inTopLeft, FVector inTopRight, FVector Dimensions, float inMinimumDistance, float inRejectionDistance) 
{
  // new settings
  UPoissonSettings poissonSettings;

  poissonSettings.TopLeft = inTopLeft;
  poissonSettings.LowerRight = inLowerRight;
  poissonSettings.Dimensions = inDimensions;
  poissonSettings.Center = (inTopLeft + inLowerRight) / 2;
  poissonSettings.CellSize =  inMinimumDistance / SquareRootTwo;
  poissonSettings.MinimumDistance = inMinimumDistance;
 // poissonSettings.RejectionSqDistance = inRejectionDistance;
  
  // == null
  //        ? null
  //        : inRejectionDistance *inRejectionDistance 
    poissonSettings.GridWidth =(int)(poissonSettings.Dimensions.X / settings.CellSize) + 1;
    poissonSettings.settings.GridHeight =(int)(poissonSettings.Dimensions.Y / settings.CellSize) + 1;
    poissonSettings.settings.GridLength = (int)(poissonSettings.Dimensions.Z / settings.CellSize) + 1;

   // state
   UState state;

    // Add first point
    AddFirstPoint(poissonSettings, state);

  // if active points are still available count
  while (state.ActivePoints.Count != 0) 
  {
    int32  listIndex = RandomHelper.Random.Next(state.ActivePoints.Count);

    FVector point = state.ActivePoints[listIndex];
    bool found = false;

    for (var k = 0; k < pointsPerIteration; k++)
      found |= AddNextPoint(point, ref settings, ref state);

    if (!found)
      state.ActivePoints.RemoveAt(listIndex);
  }

  return state.Points;
}

void PoissonSampler::AddFirstPoint(PoissonSettings inSettings,const FState &inState)
{
  // do added to certain points
  bool added = false;

  while (!added) {
    // Select a spot
    float d = RandomFloat();
    float xR = inSettings.TopLeft.X + (inSettings.Dimensions.X * d);

    d = RandomFloat();
    float yR = inSettings.TopLeft.Y + (inSettings.Dimensions.Y * d);

    d = RandomFloat();
    float zR = inSettings.TopLeft.Y + (inSettings.Dimensions.Z * d);

    // create a point
    FVector p = new FVector((float)xR, (float)yR, (float)zR);

    // check if distance square between points
    if (settings.RejectionSqDistance != null &&
        FVector.DistanceSquared(inSettings.Center.Center, p) >
            settings.RejectionSqDistance)
      continue;

    // added
    added = true;

    // continue if added
    FVector3 index = Denormalize(p, inSettings.Origin, inSettings.CellSize);

    // use index values;
                inState.Grid[(int)index.X+((int)index.Y*inSettings.gridWidth)+((int)index.Z*(inSettings.gridWidth*inSettings.gridLength)]= p;

                // add points
                inState.ActivePoints.Add(p);
                inState.Points.Add(p);
  }
}


bool PoissonSampler::AddNextPoint(Vector3 point, UPoissonSettings inSettings, const UState & state)
	    {
		    var found = false;
            var q = GenerateRandomAround(point, settings.MinimumDistance);

            if (q.X >= inSettings.TopLeft.X && q.X < inSettings.LowerRight.X && 
                q.Y > inSettings.TopLeft.Y && q.Y < inSettings.LowerRight.Y &&
                q.Z > inSettings.TopLeft.Z && q.Z < inSettings.LowerRight.Z &&
                (inSettings.RejectionSqDistance == null || Vector3.DistanceSquared(inSettings.Center, q) <= inSettings.RejectionSqDistance))
		    {
                var qIndex = Denormalize(q, settings.TopLeft, settings.CellSize);
			    var tooClose = false;

                for (var i = (int)Math.Max(0, qIndex.X - 2); i < Math.Min(inSettings.GridWidth, qIndex.X + 3) && !tooClose; i++)
                {
                    for (var j = (int)Math.Max(0, qIndex.Y - 2); j < Math.Min(inSettings.GridLength, qIndex.Y + 3) && !tooClose; j++)
                    {
                        for (var k = (int)Math.Max(0, qIndex.Z - 2); k < Math.Min(inSettings.GridHeight, qIndex.Z + 3) && !tooClose; k++)
                        {
					    if (state.Grid[CalulateIndex(inSettings, i,j,k)].HasValue && Vector3.Distance(state.Grid[CalulateIndex(inSettings, i,j,k)].Value, q) <inSettings.MinimumDistance)
							{
                                tooClose = true;
                            }
                        }
                    }
                }

			    if (!tooClose)
			    {
				    found = true;
				    state.ActivePoints.Add(q);
				    state.Points.Add(q);
                    state.Grid[(int)qIndex.X, (int)qIndex.Y, (int) qIndex.Z] = q;
			    }
		    }
		    return found;
	    }




FVector PoissonSampler::Denormalize(FVector inPoint, FVector inOrigin,
                                    double inCellSize) {
  return FVector((int)((inPoint.x - inOrigin.x) / inCellSize),
                 (int)((inPoint.y - inOrigin.y) / inCellSize),
                 (int)((inPoint.z - inOrigin.z) / inCellSize));
}

float PoissonSampler::RandomFloat() { fstream return floatValue; }


static Vector2 GenerateRandomAround(Vector2 center, float minimumDistance)
        {
            float rd1 =
            float rd2
            float rd3

            //mindset  to 2
            radius =mindist*(rd1+1);

            angle1 = 2 * PO  * rd2
            angle2 = 2 * PI * rd3
            var d = RandomHelper.Random.NextDouble();
            var radius = minimumDistance + minimumDistance * d;

            d = RandomHelper.Random.NextDouble();
            var angle = MathHelper.TwoPi * d;

            var newX = radius * Math.Sin(angle);
            var newY = radius * Math.Cos(angle);

              //the new point is generated around the point (x, y, z)
            newX = radius * cos(angle1) * sin(angle2);
            newY = radius * sin(angle1) * sin(angle2);
            newZ = radius * cos(angle2);


            return new Vector3((float) (center.X + newX), (float) (center.Y + newY), (center.Z + newZ));
        }



int32 PoissonSampler::CalculateIndex(UPoissonSettings inSetting, uint32 i, uint32 j, uint32 k)
{

    return i+(j*inSettings.gridWidth)+(k*(inSettings.gridWidth*inSettings.gridLength));    
}


