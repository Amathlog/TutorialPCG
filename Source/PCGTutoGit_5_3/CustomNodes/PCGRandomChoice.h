#pragma once

#include "PCGSettings.h"
#include "PCGPin.h"

#include "PCGRandomChoice.generated.h"

UCLASS(BlueprintType)
class UPCGRandomChoiceSettings : public UPCGSettings
{
    GENERATED_BODY()
public:
    UPCGRandomChoiceSettings();

    //~Begin UPCGSettings interface
#if WITH_EDITOR
    virtual FName GetDefaultNodeName() const override { return FName(TEXT("NativeRandomChoice")); }
    virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("PCGRandomChoiceSettings", "NodeTitle", "Native Random Choice"); }
    virtual FText GetNodeTooltipText() const override { return NSLOCTEXT("PCGRandomChoiceSettings", "NodeTooltip", "Split a point input in 2, randomly according to a ratio."); }
    virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Sampler; }
#endif

    virtual FName AdditionalTaskName() const override;

protected:
    virtual TArray<FPCGPinProperties> InputPinProperties() const override { return Super::DefaultPointInputPinProperties(); }
    virtual TArray<FPCGPinProperties> OutputPinProperties() const override;
    virtual FPCGElementPtr CreateElement() const override;
    //~End UPCGSettings interface

public:
    /** If True, will use the fixed number of points. Ratio otherwise. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable))
    bool bFixedMode = false;

    /** Ratio of points to keep. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable, ClampMin = 0.0, ClampMax = 1.0, EditCondition = "!bFixedMode", EditConditionHides))
    float Ratio = 0.1f;

    /** Fixed number of points to keep. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings", meta = (PCG_Overridable, ClampMin = 1, EditCondition = "bFixedMode", EditConditionHides))
    int32 FixedNumber = true;
};

class FPCGRandomChoiceElement : public FSimplePCGElement
{
protected:
    virtual bool ExecuteInternal(FPCGContext* Context) const override;
};