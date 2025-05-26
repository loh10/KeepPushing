#include "Timer/TimerUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UTimerUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Timer && TimerTextBlock)
	{
		TimerTextBlock->SetText(FText::FromString(Timer->GetFormattedTime()));
	}
}

void UTimerUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
		StartButton->OnClicked.AddDynamic(this, &UTimerUserWidget::OnStartClicked);

	if (PauseButton)
		PauseButton->OnClicked.AddDynamic(this, &UTimerUserWidget::OnPauseClicked);

	if (ResetButton)
		ResetButton->OnClicked.AddDynamic(this, &UTimerUserWidget::OnResetClicked);
}

void UTimerUserWidget::OnStartClicked()
{
	if (Timer)
		Timer->StartTimer();
}

void UTimerUserWidget::OnPauseClicked()
{
	if (Timer)
		Timer->StopTimer();
}

void UTimerUserWidget::OnResetClicked()
{
	if (Timer)
		Timer->ResetTimer();
}
