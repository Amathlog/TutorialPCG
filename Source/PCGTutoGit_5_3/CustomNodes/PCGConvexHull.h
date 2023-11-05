#pragma once

#include "PCGSettings.h"
#include "PCGPin.h"

#include "PCGConvexHull.generated.h"

UCLASS(BlueprintType)
class UPCGConvexHullSettings : public UPCGSettings
{
    GENERATED_BODY()
public:
    //~Begin UPCGSettings interface
#if WITH_EDITOR
    virtual FName GetDefaultNodeName() const override { return FName(TEXT("FindConvexHull")); }
    virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("PCGConvexHullSettings", "NodeTitle", "Find Convex Hull"); }
    virtual FText GetNodeTooltipText() const override { return NSLOCTEXT("PCGConvexHullSettings", "NodeTooltip", "Return the convex hull of a set of points"); }
    virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Spatial; }
#endif


protected:
    virtual TArray<FPCGPinProperties> InputPinProperties() const override { return Super::DefaultPointInputPinProperties(); }
    virtual TArray<FPCGPinProperties> OutputPinProperties() const override { return Super::DefaultPointOutputPinProperties(); }
    virtual FPCGElementPtr CreateElement() const override;
    //~End UPCGSettings interface

public:
    /** Points are sorted in clockwise order. If False, sorted in counter clocckwise order. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable))
    bool bClockwise = true;
};

class FPCGConvexHullElement : public FSimplePCGElement
{
protected:
    virtual bool ExecuteInternal(FPCGContext* Context) const override;
};