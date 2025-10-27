#pragma once

#include "CoreMinimal.h"
#include "EngineGlobals.h"   // GEngine 전역 포인터 선언
#include "Engine/Engine.h"   // UEngine 정의 및 AddOnScreenDebugMessage

namespace Debug
{
    // InKey의 경우 같은값을 주면 아웃풋에서 덮어씌워짐(프레임별 디버그)
    // -1 값은 계속 새로운 디버그 출력
    static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
    {
        if(GEngine)
        {
            // 디버그 메시지를 게임화면에 띄움
            GEngine->AddOnScreenDebugMessage(InKey,7.f,Color,Msg);
            
            // *Msg 를 Text로 로그 출력
            UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);   
        }
    }

    static void Print(const FString& FloatTitle, float FloatValueToPrint, int32 InKey = -1, const FColor& Color = FColor::MakeRandomColor())
    {
        if (GEngine)
        {
            const FString FinalMsg = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);

            GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, FinalMsg);
            
            UE_LOG(LogTemp,Warning,TEXT("%s"), *FinalMsg);   
        }
    }
}