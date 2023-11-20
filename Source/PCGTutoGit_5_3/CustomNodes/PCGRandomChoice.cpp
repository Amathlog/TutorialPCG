#include "PCGRandomChoice.h"

#include "PCGContext.h"
#include "PCGModule.h"
#include "PCGParamData.h"
#include "Data/PCGPointData.h"

#define LOCTEXT_NAMESPACE "PCGConvexHullSettings"

namespace PCGRandomChoiceSettings
{
    static const FName ChosenPointsLabel = TEXT("ChosenPoints");
    static const FName DiscardedPointsLabel = TEXT("DiscardedPoints");
    static const FName ResultsLabel = TEXT("Results");
}

UPCGRandomChoiceSettings::UPCGRandomChoiceSettings()
{
    bUseSeed = true;
}

FName UPCGRandomChoiceSettings::AdditionalTaskName() const
{
    return NAME_None;
}

TArray<FPCGPinProperties> UPCGRandomChoiceSettings::OutputPinProperties() const
{
    TArray<FPCGPinProperties> Properties;

    Properties.Emplace(PCGRandomChoiceSettings::ChosenPointsLabel, EPCGDataType::Point);
    Properties.Emplace(PCGRandomChoiceSettings::DiscardedPointsLabel, EPCGDataType::Point);
    Properties.Emplace(PCGRandomChoiceSettings::ResultsLabel, EPCGDataType::Param);

    return Properties;
}

FPCGElementPtr UPCGRandomChoiceSettings::CreateElement() const
{
    return MakeShared<FPCGRandomChoiceElement>();
}

bool FPCGRandomChoiceElement::ExecuteInternal(FPCGContext* Context) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FPCGRandomChoiceElement::Execute);

    check(Context);

    const UPCGRandomChoiceSettings* Settings = Context->GetInputSettings<UPCGRandomChoiceSettings>();
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

        const TArray<FPCGPoint>& InPoints = PointData->GetPoints();

        // Compute the number of elements to keep
        int32 NumberOfElementsToKeep = 0;
        if (!Settings->bFixedMode)
        {
            float Ratio = FMath::Clamp(Settings->Ratio, 0.0f, 1.0f);
            NumberOfElementsToKeep = FMath::CeilToInt(Ratio * InPoints.Num());
        }
        else
        {
            NumberOfElementsToKeep = FMath::Clamp(Settings->FixedNumber, 1, InPoints.Num() - 1);
        }
        
        TArray<int32> Indexes;
        Indexes.Reserve(InPoints.Num());
        for (int32 i = 0; i < InPoints.Num(); ++i)
        {
            Indexes.Add(i);
        }

        FRandomStream RandomStream(Context->GetSeed());

        // Shuffle
        int32 LastIndex = InPoints.Num() - 1;
        for (int32 i = 0; i <= LastIndex; ++i)
        {
            const int32 Index = RandomStream.RandRange(i, LastIndex);
            if (i != Index)
            {
                Swap(Indexes[i], Indexes[Index]);
            }
        }

        // Create output point data
        UPCGPointData* ChosenPointsData = NewObject<UPCGPointData>();
        ChosenPointsData->InitializeFromData(PointData);
        TArray<FPCGPoint>& ChosenPoints = ChosenPointsData->GetMutablePoints();
        ChosenPoints.Reserve(NumberOfElementsToKeep);

        UPCGPointData* DiscardedPointsData = NewObject<UPCGPointData>();
        DiscardedPointsData->InitializeFromData(PointData);
        TArray<FPCGPoint>& DiscardedPoints = DiscardedPointsData->GetMutablePoints();
        DiscardedPoints.Reserve(InPoints.Num() - NumberOfElementsToKeep);

        for (int32 i = 0; i < InPoints.Num(); ++i)
        {
            if (i < NumberOfElementsToKeep)
            {
                ChosenPoints.Add(InPoints[Indexes[i]]);
            }
            else
            {
                DiscardedPoints.Add(InPoints[Indexes[i]]);
            }
        }

        // Create Attribute set
        UPCGParamData* ParamData = NewObject<UPCGParamData>();
        FPCGMetadataAttribute<int32>* Attribute = ParamData->Metadata->CreateAttribute<int32>(TEXT("ChosenPointsNum"), NumberOfElementsToKeep, true, true);
        Attribute->SetValue(ParamData->Metadata->AddEntry(), NumberOfElementsToKeep);

        // Output all in output collection
        FPCGTaggedData& ChosenTaggedData = Outputs.Add_GetRef(Input);
        ChosenTaggedData.Data = ChosenPointsData;
        ChosenTaggedData.Pin = PCGRandomChoiceSettings::ChosenPointsLabel;

        FPCGTaggedData& DiscardedTaggedData = Outputs.Add_GetRef(Input);
        DiscardedTaggedData.Data = DiscardedPointsData;
        DiscardedTaggedData.Pin = PCGRandomChoiceSettings::DiscardedPointsLabel;

        FPCGTaggedData& ResultsTaggedData = Outputs.Add_GetRef(Input);
        ResultsTaggedData.Data = ParamData;
        ResultsTaggedData.Pin = PCGRandomChoiceSettings::ResultsLabel;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE