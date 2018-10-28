﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace Control
{
    class SineDrawer
    {
        Canvas canvas;
        MainWindowState state;

        double width;
        double height;
        int periodsCount;
        double sawOffsetY;
        double sawHeight = 40;
        double f1Width;
        double axisXOffset;

        public SineDrawer(Canvas canvas, MainWindowState state)
        {
            this.canvas = canvas;
            this.state = state;
        }


        void add(UIElement element)
        {
            add(element, 2);
        }

        void add(UIElement element, int zIndex)
        {
            Canvas.SetZIndex(element, zIndex);
            canvas.Children.Add(element);
        }

        void drawBackground()
        {
            //draw background
            Rectangle backgroundRectangle = new Rectangle();
            backgroundRectangle.Width = width;
            backgroundRectangle.Height = height;
            backgroundRectangle.Stroke = new SolidColorBrush(Colors.DarkGray);
            backgroundRectangle.StrokeThickness = 2;
            backgroundRectangle.Fill = new SolidColorBrush(Colors.WhiteSmoke);
            backgroundRectangle.Opacity = 0.75F;
            
            // Set Canvas position    
            Canvas.SetLeft(backgroundRectangle, 0);
            Canvas.SetTop(backgroundRectangle, 0);
            Canvas.SetZIndex(backgroundRectangle, 0);
            // Add Rectangle to Canvas    
            canvas.Children.Add(backgroundRectangle);

        }


        Line createLineForSineF2()
        {
            Line l = new Line();
            l.Stroke = Brushes.Yellow;
            l.StrokeThickness = 1;
            return l;
        }

        Line createLineForSineF1()
        {
            Line l = new Line();
            l.Stroke = Brushes.Brown;
            l.StrokeThickness = 2;
            return l;
        }
        Line createLineForSaw()
        {
            Line l = new Line();
            l.Stroke = Brushes.Black;
            l.StrokeThickness = 2;
            return l;
        }


        Line createLineForPeriod()
        {
            Line l = new Line();
            l.Stroke = Brushes.Green;
            l.StrokeThickness = 1;
            return l;
        }

        void drawSaw()
        {
            sawOffsetY = height - sawHeight - 10;

            //draw F1 Saw
            double sawOffsetX = 0; //use cosinus instead of  f1Width / 4;
            double x = sawOffsetX;
            double y = sawOffsetY;
            double sawStepWidth = f1Width / 2;
            for (int i = 0; i < periodsCount; i++)
            {
                if (i == periodsCount / 3)
                {
                    //draw period
                    Line periodStart = createLineForPeriod();
                    periodStart.X1 = x;
                    periodStart.X2 = x;
                    periodStart.Y1 = 0;
                    periodStart.Y2 = height;
                    add(periodStart,1);

                    Line periodStop = createLineForPeriod();
                    periodStop.X1 = x + f1Width;
                    periodStop.X2 = x + f1Width;
                    periodStop.Y1 = 0;
                    periodStop.Y2 = height;
                    add(periodStop, 1);
                }



                //draw top horizontal
                Line l = createLineForSaw();
                l.X1 = x;
                l.X2 = x + sawStepWidth;
                l.Y1 = y;
                l.Y2 = y;
                add(l);
                x += sawStepWidth;

                //draw vertical
                l = createLineForSaw();
                l.X1 = x;
                l.X2 = x;
                l.Y1 = y;
                l.Y2 = y + sawHeight;
                add(l);
                y += sawHeight;

                //draw bottom horizontal
                l = createLineForSaw();
                l.X1 = x;
                l.X2 = x + sawStepWidth;
                l.Y1 = y;
                l.Y2 = y;
                add(l);
                x += sawStepWidth;

                //draw vertical
                l = createLineForSaw();
                l.X1 = x;
                l.X2 = x;
                l.Y1 = y;
                l.Y2 = y - sawHeight;
                add(l);
                y -= sawHeight;

            }
        }

        public static double ToRadians(double angle)
        {
            return (angle * Math.PI) / 180;
        }

        private void drawSineF2()
        {
            double eF = 190;
            double x1=0;
            double y1 = Math.Sin(0) * eF + axisXOffset;
            double y2 = 0;
            double x2 = 0;

            double y1_m = -Math.Sin(0) * eF + axisXOffset;
            double y2_m;
            

            for (double x=1; x <= width; x++)
            {
                double angle = 180F * (float)(x / width);
                double a = ToRadians(angle);
                double y2_withoutOffset = Math.Sin(a) * eF;
                y2 = y2_withoutOffset + axisXOffset;
                x2 = x;
                heightList[(int)x] = (int)y2_withoutOffset;

                Line l = createLineForSineF2();
                l.X1 = x1;
                l.Y1 = y1;
                l.X2 = x2;
                l.Y2 = y2;
                //line above
                add(l);

                //line below
                y2_m = -Math.Sin(a) * eF + axisXOffset;
                l = createLineForSineF2();
                l.X1 = x1;
                l.Y1 = y1_m;
                l.X2 = x2;
                l.Y2 = y2_m;
                add(l);

                x1 = x2;
                y1 = y2;
                y1_m = y2_m;
            }
        }

        private void drawSineF1()
        {
            double eF = 30;
            double x1 = 0;
            double y1 = Math.Sin(0) * eF + axisXOffset;
            double y2 = 0;
            double x2 = 0;

            double xDivider = f1Width / 360F;
            int step = (int)f1Width;
            int startOffset = -(step / 4) + 2;
            for (int x = startOffset; x < width; x += step)
            {
                x1 = x;
                for (double angle = 1; angle <= 360; angle++)
                {
                    double a = ToRadians(angle);
                    y2 = Math.Sin(a) * eF + axisXOffset;
                    x2 = x + (angle * xDivider);
                    if ((int)x2 == (int)x1)
                        continue;

                    Line l = createLineForSineF1();
                    l.X1 = x1;
                    l.Y1 = y1;
                    l.X2 = x2;
                    l.Y2 = y2;
                    add(l);

                    x1 = x2;
                    y1 = y2;
                }
            }
        }

        //F2 (yellow) sine height for each pixel
        int[] heightList;
        public void Draw()
        {

            width = canvas.ActualWidth;
            height = canvas.ActualHeight;
            periodsCount = state.GetPrescaler();
            f1Width = width / periodsCount;
            heightList = new int[(int)width + 1];
            axisXOffset = (height - sawHeight) / 2;

            canvas.Children.Clear();

            drawBackground();

            drawSaw();

            drawSineF2();

            drawSineF1();
            
        }


    }
}
