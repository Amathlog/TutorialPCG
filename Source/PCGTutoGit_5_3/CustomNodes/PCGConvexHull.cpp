#include "PCGConvexHull.h"

#include "PCGContext.h"
#include "PCGModule.h"
#include "Data/PCGPointData.h"

#include "Algo/MinElement.h"

#define LOCTEXT_NAMESPACE "PCGConvexHullSettings"

FPCGElementPtr UPCGConvexHullSettings::CreateElement() const
{
    return MakeShared<FPCGConvexHullElement>();
}

bool FPCGConvexHullElement::ExecuteInternal(FPCGContext* Context) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FPCGConvexHullElement::Execute);

    check(Context);

    const UPCGConvexHullSettings* Settings = Context->GetInputSettings<UPCGConvexHullSettings>();
    check(Settings);

    TArray<FPCGTaggedData>& Outputs = Context->OutputData.TaggedData;

    for (const FPCGTaggedData& Input : Context->InputData.GetInputsByPin(PCGPinConstants::DefaultInputLabel))
    {
        const UPCGPointData* PointData = Cast<UPCGPointData>(Input.Data);

        if (!PointData)
        {
            PCGE_LOG(Error, GraphAndLog, LOCTEXT("InputNotPointData", "Input is not a point data"));
            continue;
        }

        // Algorithm: https://en.wikipedia.org/wiki/Graham_scan
        // Complexity: O(n*log(n)) (depends on the sort algorithm)

        // First: Find the point with the smallest Y -> Named P0
        const TArray<FPCGPoint>& Points = PointData->GetPoints();
        const FPCGPoint* P0 = Algo::MinElementBy(Points, [](const FPCGPoint& Point) { return Point.Transform.GetLocation().Y; });

        if (!P0)
        {
            continue;
        }

        // Second: Sort the points in descending order, on the X value of the difference between the point and the MinPoint. If 2 points have the same dot product, use the distance (only on XY).
        TArray<int32> Indexes;
        Indexes.Reserve(Points.Num());
        for (int32 i = 0; i < Points.Num(); ++i)
        {
            Indexes.Add(i);
        }

        const bool bClockwise = Settings->bClockwise;

        Indexes.Sort([&Points, P0, bClockwise](int32 i, int32 j) -> bool
            {
                // p0 is always the smallest
                if (&Points[i] == P0 || &Points[j] == P0)
                {
                    return &Points[i] == P0;
                }

                FVector2D First(Points[i].Transform.GetLocation() - P0->Transform.GetLocation());
                FVector2D Second(Points[j].Transform.GetLocation() - P0->Transform.GetLocation());

                const double FirstDistance = First.SquaredLength();
                const double SecondDistance = First.SquaredLength();

                First.Normalize();
                Second.Normalize();

                bool bClockwiseCondition = bClockwise ? (First.X > Second.X) : (First.X < Second.X);

                return bClockwiseCondition || (FMath::IsNearlyEqual(First.X, Second.X) && FirstDistance > SecondDistance);

            });

        // Third: Construct the convex hull by iterating on the sorted array

        // Return > 0 if ABC is the wanted order (clockwise or counterclockwise), =0 if they are collinear, < 0 if it is in the wrong order.
        auto IsInRightOrder = [bClockwise](const FPCGPoint& A, const FPCGPoint& B, const FPCGPoint& C) -> double
            {
                const FVector2D AB(B.Transform.GetLocation() - A.Transform.GetLocation());
                const FVector2D BC(C.Transform.GetLocation() - B.Transform.GetLocation());

                double CrossProduct = FVector2D::CrossProduct(AB, BC);
                return bClockwise ? CrossProduct : -CrossProduct;
            };

        TArray<int32> Stack;
        for (int32 Index : Indexes)
        {
            while (Stack.Num() > 1 && IsInRightOrder(Points[Stack[Stack.Num() - 2]], Points[Stack[Stack.Num() - 1]], Points[Index]) <= 0.0)
            {
                Stack.Pop();
            }

            Stack.Add(Index);
        }

        FPCGTaggedData& Output = Outputs.Add_GetRef(Input);
        UPCGPointData* OutputPointData = NewObject<UPCGPointData>();
        OutputPointData->InitializeFromData(PointData);
        TArray<FPCGPoint>& OutPoints = OutputPointData->GetMutablePoints();
        OutPoints.Reserve(Stack.Num());
        for (int32 Index : Stack)
        {
            OutPoints.Add(Points[Index]);
        }

        Output.Data = OutputPointData;
        Output.Pin = PCGPinConstants::DefaultOutputLabel;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE